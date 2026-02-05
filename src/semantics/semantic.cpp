#include "semantic.h"
#include <stdexcept>
#include <map>

// Helper to convert TypeKind to string
std::string typeToString(const Type& t) {
    switch (t.kind) {
        case TypeKind::I8: return "i8";
        case TypeKind::I16: return "i16";
        case TypeKind::I32: return "i32";
        case TypeKind::I64: return "i64";
        case TypeKind::F32: return "f32";
        case TypeKind::F64: return "f64";
        case TypeKind::String: return "string";
        case TypeKind::List:
            if (t.element) {
                return "list<" + typeToString(*t.element) + ">";
            }
            return "list<unknown>"; // Should not happen if well-formed
        default: return "unknown";
    }
}

// Helper to check type compatibility
bool isCompatible(const Type& t1, const Type& t2) {
    if (t1.kind == t2.kind) {
        if (t1.kind == TypeKind::List) {
            if (!t1.element || !t2.element) return false; // Should not happen if types are well-formed
            return isCompatible(*t1.element, *t2.element);
        }
        return true;
    }

    // Implicit conversions (e.g., i8 to i32, i32 to f64)
    if ((t1.kind == TypeKind::I8 || t1.kind == TypeKind::I16 || t1.kind == TypeKind::I32 || t1.kind == TypeKind::I64) &&
        (t2.kind == TypeKind::I8 || t2.kind == TypeKind::I16 || t2.kind == TypeKind::I32 || t2.kind == TypeKind::I64)) {
        return true; // All integer types are compatible with each other for now
    }
    if ((t1.kind == TypeKind::F32 || t1.kind == TypeKind::F64) &&
        (t2.kind == TypeKind::F32 || t2.kind == TypeKind::F64)) {
        return true; // All float types are compatible
    }
    if ((t1.kind == TypeKind::I8 || t1.kind == TypeKind::I16 || t1.kind == TypeKind::I32 || t1.kind == TypeKind::I64) &&
        (t2.kind == TypeKind::F32 || t2.kind == TypeKind::F64)) {
        return true; // Int to float conversion
    }
    if ((t2.kind == TypeKind::I8 || t2.kind == TypeKind::I16 || t2.kind == TypeKind::I32 || t2.kind == TypeKind::I64) &&
        (t1.kind == TypeKind::F32 || t1.kind == TypeKind::F64)) {
        return true; // Int to float conversion
    }

    return false;
}

// Helper for reporting errors
void SemanticAnalyzer::error(const std::string& message, int line, int column) {
    std::cerr << "Semantic Error at line " << line << ", column " << column << ": " << message << std::endl;
    exit(1);
}

// Main analysis function
void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Stmt>>& program) {
    for (const auto& stmt : program) {
        if (auto letStmt = dynamic_cast<const LetStmt*>(stmt.get())) {
            Type exprType = analyzeExpr(letStmt->expr.get());

            // Determine the actual type to store in the symbol table
            Type actualStoredType;
            // If a type was explicitly declared (i.e., not the default unknownType() from constructor)
            bool typeExplicitlyDeclared = (letStmt->declared_type.kind != TypeKind::Unknown);

            if (typeExplicitlyDeclared) {
                if (!isCompatible(letStmt->declared_type, exprType)) {
                    error("Type mismatch in variable declaration. Expected " + 
                          typeToString(letStmt->declared_type) + ", got " + 
                          typeToString(exprType), letStmt->expr->line, letStmt->expr->column);
                }
                actualStoredType = letStmt->declared_type;
            } else {
                // No explicit type, infer from expression
                actualStoredType = exprType;
            }
            // Store the type in the symbol table
            symbols.set(letStmt->name, Symbol{letStmt->name, actualStoredType, 0, 0, "", std::vector<Symbol>()});
        } else if (auto printStmt = dynamic_cast<const PrintStmt*>(stmt.get())) {
            analyzeExpr(printStmt->expr.get()); // Just analyze for type correctness
        }
        // Add other statement types here
    }
}

// Expression analysis
Type SemanticAnalyzer::analyzeExpr(const Expr* expr) {
    if (!expr) {
        // This should be an error case, but for now return a default
        return i32Type();
    }

    switch (expr->kind) {
        case ExprKind::NumberLiteral:
        case ExprKind::StringLiteral:
            return expr->type; // Type already set by parser
        
        case ExprKind::Identifier: {
            // Look up identifier in symbol table
            if (!symbols.exists(expr->string_value)) {
                error("Undeclared identifier: " + expr->string_value, expr->line, expr->column);
            }
            // Assign the type from the symbol table to the expression node
            // Note: This modifies the AST during semantic analysis
            const_cast<Expr*>(expr)->type = symbols.get(expr->string_value).type;
            return symbols.get(expr->string_value).type;
        }
        
        case ExprKind::Binary: {
            Type leftType = analyzeExpr(expr->left.get());
            Type rightType = analyzeExpr(expr->right.get());

            // Determine result type and check compatibility based on operator
            if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%" || expr->op == "//") {
                if (isCompatible(leftType, rightType)) {
                    // Promote type if necessary (e.g., i32 + f64 = f64)
                    if (leftType.kind == TypeKind::F64 || rightType.kind == TypeKind::F64) {
                        const_cast<Expr*>(expr)->type = f64Type();
                        return f64Type();
                    } else if (leftType.kind == TypeKind::F32 || rightType.kind == TypeKind::F32) {
                        const_cast<Expr*>(expr)->type = f32Type();
                        return f32Type();
                    } else {
                        // All integers promote to i32 for now
                        const_cast<Expr*>(expr)->type = i32Type();
                        return i32Type();
                    }
                } else {
                    error("Incompatible types for binary arithmetic operation '" + expr->op + "': " + 
                          typeToString(leftType) + " and " + typeToString(rightType), expr->line, expr->column);
                }
            } else if (expr->op == "==" || expr->op == "!=" || expr->op == "<" || expr->op == "<=" || expr->op == ">" || expr->op == ">=") {
                if (isCompatible(leftType, rightType)) {
                    const_cast<Expr*>(expr)->type = i32Type(); // Comparison results in boolean (represented as i32)
                    return i32Type();
                } else {
                    error("Incompatible types for binary comparison operation '" + expr->op + "': " + 
                          typeToString(leftType) + " and " + typeToString(rightType), expr->line, expr->column);
                }
            } else if (expr->op == "&&" || expr->op == "||") {
                // Logical operations expect booleans (i32)
                if ((leftType.kind == TypeKind::I32 || leftType.kind == TypeKind::I8 || leftType.kind == TypeKind::I16) && 
                    (rightType.kind == TypeKind::I32 || rightType.kind == TypeKind::I8 || rightType.kind == TypeKind::I16)) {
                    const_cast<Expr*>(expr)->type = i32Type();
                    return i32Type();
                } else {
                    error("Incompatible types for binary logical operation '" + expr->op + "': expected integer types, got " + 
                          typeToString(leftType) + " and " + typeToString(rightType), expr->line, expr->column);
                }
            } else {
                error("Unknown binary operator: " + expr->op, expr->line, expr->column);
            }
            break;
        }
        
        case ExprKind::Unary: {
            Type operandType = analyzeExpr(expr->left.get());
            if (expr->op == "-" || expr->op == "!") {
                if ((operandType.kind == TypeKind::I32 || operandType.kind == TypeKind::I8 || operandType.kind == TypeKind::I16) || 
                    (operandType.kind == TypeKind::F32 || operandType.kind == TypeKind::F64)) {
                    const_cast<Expr*>(expr)->type = operandType; // Unary - preserves type
                    return operandType;
                } else {
                    error("Incompatible type for unary operation '" + expr->op + "': expected numeric type, got " + 
                          typeToString(operandType), expr->line, expr->column);
                }
            } else {
                error("Unknown unary operator: " + expr->op, expr->line, expr->column);
            }
            break;
        }

        case ExprKind::ListLiteral: {
            if (expr->elements.empty()) {
                const_cast<Expr*>(expr)->type = listType(new Type(i32Type())); // Default empty list to list<i32>
                return listType(new Type(i32Type()));
            }

            Type firstElementType = analyzeExpr(expr->elements[0].get());
            for (size_t i = 1; i < expr->elements.size(); ++i) {
                Type currentElementType = analyzeExpr(expr->elements[i].get());
                if (!isCompatible(firstElementType, currentElementType)) {
                    error("List elements must be of compatible types. Expected " + 
                          typeToString(firstElementType) + ", got " + 
                          typeToString(currentElementType), expr->elements[i]->line, expr->elements[i]->column);
                }
            }
            const_cast<Expr*>(expr)->type = listType(new Type(firstElementType));
            return listType(new Type(firstElementType));
        }

        case ExprKind::StringSlice:
        case ExprKind::MethodCall:
            // Need to implement type checking for these
            // For now, assume they return string or i32
            return stringType(); // Placeholder
        
        default:
            return i32Type(); // Default or error type
    }
    // Should not reach here, but to satisfy compiler
    return i32Type();
}