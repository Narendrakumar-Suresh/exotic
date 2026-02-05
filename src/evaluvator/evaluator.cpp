#include "evaluator.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

Evaluator::Evaluator(SymbolTable& symbols) : symbols(symbols) {}

void Evaluator::evalProgram(const std::vector<std::unique_ptr<Stmt>>& program) {
    for (const auto& stmt : program) {
        if (auto letStmt = dynamic_cast<const LetStmt*>(stmt.get())) {
            Symbol result = evalExpr(letStmt->expr.get());
            symbols.set(letStmt->name, result);
        } else if (auto printStmt = dynamic_cast<const PrintStmt*>(stmt.get())) {
            evalPrintStmt(printStmt);
        }
    }
}

void Evaluator::evalPrintStmt(const PrintStmt* stmt) {
    Symbol value = evalExpr(stmt->expr.get());
    printSymbol(value);
    cout << endl;
}

void Evaluator::printSymbol(const Symbol& value) {
    switch (value.type.kind) {
        case TypeKind::I32:
        case TypeKind::I8:
        case TypeKind::I16:
            cout << value.int_value;
            break;
        case TypeKind::F32:
        case TypeKind::F64:
            cout << value.double_value;
            break;
        case TypeKind::String:
            cout << value.string_value;
            break;
        case TypeKind::List:
            cout << "[";
            for (size_t i = 0; i < value.list_values.size(); ++i) {
                printSymbol(value.list_values[i]);
                if (i < value.list_values.size() - 1) {
                    cout << ", ";
                }
            }
            cout << "]";
            break;
        default:
            cout << "Unknown type to print";
            break;
    }
}

Symbol Evaluator::evalExpr(const Expr* expr) {
    switch (expr->kind) {
        case ExprKind::NumberLiteral: {
            Symbol sym;
            if (expr->type.kind == TypeKind::F64) {
                sym.type = f64Type();
                sym.double_value = expr->double_value;
            } else {
                sym.type = i32Type();
                sym.int_value = expr->int_value;
            }
            return sym;
        }
        
        case ExprKind::StringLiteral: {
            Symbol sym;
            sym.type = stringType();
            sym.string_value = expr->string_value;
            return sym;
        }
        
        case ExprKind::Identifier: {
            return symbols.get(expr->string_value);
        }
        
        case ExprKind::Binary:
            return evalBinary(expr);
            
        case ExprKind::Unary:
            return evalUnary(expr);
            
        case ExprKind::StringSlice:
            return evalStringSlice(expr);
            
        case ExprKind::MethodCall:
            return evalMethodCall(expr);
            
        case ExprKind::ListLiteral: {
            Symbol sym;
            sym.type = listType(nullptr); // Placeholder, will set actual element type below
            
            if (!expr->elements.empty()) {
                for (const auto& elementExpr : expr->elements) {
                    sym.list_values.push_back(evalExpr(elementExpr.get()));
                }
                sym.type = listType(new Type(sym.list_values[0].type));
            } else {
                sym.type = listType(new Type(i32Type())); // Default for empty list
            }
            return sym;
        }
    }
    
    Symbol err;
    err.type = i32Type();
    err.int_value = 0;
    return err;
}

Symbol Evaluator::evalBinary(const Expr* expr) {
    Symbol left = evalExpr(expr->left.get());
    Symbol right = evalExpr(expr->right.get());
    Symbol result;
    
    if (expr->op == "+") {
        if (left.type.kind == TypeKind::String || right.type.kind == TypeKind::String) {
            result.type = stringType();
            result.string_value = left.string_value + right.string_value;
        } else if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            result.type = f64Type();
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.double_value = lval + rval;
        } else {
            result.type = i32Type();
            result.int_value = left.int_value + right.int_value;
        }
    } else if (expr->op == "-") {
        if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            result.type = f64Type();
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.double_value = lval - rval;
        } else {
            result.type = i32Type();
            result.int_value = left.int_value - right.int_value;
        }
    } else if (expr->op == "*") {
        if (left.type.kind == TypeKind::String && right.type.kind == TypeKind::I32) {
            result.type = stringType();
            for (int i = 0; i < right.int_value; i++) {
                result.string_value += left.string_value;
            }
        } else if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            result.type = f64Type();
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.double_value = lval * rval;
        } else {
            result.type = i32Type();
            result.int_value = left.int_value * right.int_value;
        }
    } else if (expr->op == "/") {
        if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            result.type = f64Type();
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.double_value = lval / rval;
        } else {
            result.type = i32Type();
            result.int_value = left.int_value / right.int_value;
        }
    } else if (expr->op == "//") { // Add this block for integer division
        result.type = i32Type();
        result.int_value = left.int_value / right.int_value;
    } else if (expr->op == "%") {
        result.type = i32Type();
        result.int_value = left.int_value % right.int_value;
    } else if (expr->op == "==") {
        result.type = i32Type();
        if (left.type.kind == TypeKind::String) {
            result.int_value = (left.string_value == right.string_value) ? 1 : 0;
        } else if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.int_value = (lval == rval) ? 1 : 0;
        } else {
            result.int_value = (left.int_value == right.int_value) ? 1 : 0;
        }
    } else if (expr->op == "!=") {
        result.type = i32Type();
        if (left.type.kind == TypeKind::String) {
            result.int_value = (left.string_value != right.string_value) ? 1 : 0;
        } else if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.int_value = (lval != rval) ? 1 : 0;
        } else {
            result.int_value = (left.int_value != right.int_value) ? 1 : 0;
        }
    } else if (expr->op == "<") {
        result.type = i32Type();
        if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.int_value = (lval < rval) ? 1 : 0;
        } else {
            result.int_value = (left.int_value < right.int_value) ? 1 : 0;
        }
    } else if (expr->op == "<=") {
        result.type = i32Type();
        if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.int_value = (lval <= rval) ? 1 : 0;
        } else {
            result.int_value = (left.int_value <= right.int_value) ? 1 : 0;
        }
    } else if (expr->op == ">") {
        result.type = i32Type();
        if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.int_value = (lval > rval) ? 1 : 0;
        } else {
            result.int_value = (left.int_value > right.int_value) ? 1 : 0;
        }
    } else if (expr->op == ">=") {
        result.type = i32Type();
        if (left.type.kind == TypeKind::F64 || right.type.kind == TypeKind::F64) {
            double lval = (left.type.kind == TypeKind::F64) ? left.double_value : left.int_value;
            double rval = (right.type.kind == TypeKind::F64) ? right.double_value : right.int_value;
            result.int_value = (lval >= rval) ? 1 : 0;
        } else {
            result.int_value = (left.int_value >= right.int_value) ? 1 : 0;
        }
    } else if (expr->op == "&&") {
        result.type = i32Type();
        result.int_value = (left.int_value && right.int_value) ? 1 : 0;
    } else if (expr->op == "||") {
        result.type = i32Type();
        result.int_value = (left.int_value || right.int_value) ? 1 : 0;
    }
    
    return result;
}

Symbol Evaluator::evalUnary(const Expr* expr) {
    Symbol operand = evalExpr(expr->left.get());
    Symbol result;
    
    if (expr->op == "-") {
        if (operand.type.kind == TypeKind::F64) {
            result.type = f64Type();
            result.double_value = -operand.double_value;
        } else {
            result.type = i32Type();
            result.int_value = -operand.int_value;
        }
    } else if (expr->op == "!") {
        result.type = i32Type();
        result.int_value = !operand.int_value;
    }
    
    return result;
}

Symbol Evaluator::evalStringSlice(const Expr* expr) {
    Symbol str = evalExpr(expr->left.get());
    Symbol result;
    result.type = stringType();
    
    int start = 0;
    int end = str.string_value.length();
    
    if (expr->start) {
        Symbol startSym = evalExpr(expr->start.get());
        start = startSym.int_value;
        if (start < 0) start = str.string_value.length() + start;
    }
    
    if (expr->end) {
        Symbol endSym = evalExpr(expr->end.get());
        end = endSym.int_value;
        if (end == -1) {
            end = start + 1;
        } else if (end < 0) {
            end = str.string_value.length() + end;
        }
    }
    
    start = max(0, min(start, (int)str.string_value.length()));
    end = max(start, min(end, (int)str.string_value.length()));
    
    result.string_value = str.string_value.substr(start, end - start);
    return result;
}

Symbol Evaluator::evalMethodCall(const Expr* expr) {
    Symbol obj = evalExpr(expr->left.get());
    Symbol result;
    
    if (expr->method_name == "upper") {
        result.type = stringType();
        result.string_value = obj.string_value;
        transform(result.string_value.begin(), result.string_value.end(), 
                  result.string_value.begin(), ::toupper);
    } else if (expr->method_name == "lower") {
        result.type = stringType();
        result.string_value = obj.string_value;
        transform(result.string_value.begin(), result.string_value.end(), 
                  result.string_value.begin(), ::tolower);
    } else if (expr->method_name == "len") {
        result.type = i32Type();
        result.int_value = obj.string_value.length();
    } else if (expr->method_name == "replace") {
        if (expr->args.size() >= 2) {
            Symbol oldStr = evalExpr(expr->args[0].get());
            Symbol newStr = evalExpr(expr->args[1].get());
            
            result.type = stringType();
            result.string_value = obj.string_value;
            
            size_t pos = 0;
            while ((pos = result.string_value.find(oldStr.string_value, pos)) != string::npos) {
                result.string_value.replace(pos, oldStr.string_value.length(), newStr.string_value);
                pos += newStr.string_value.length();
            }
        }
    } else if (expr->method_name == "contains") {
        if (expr->args.size() >= 1) {
            Symbol search = evalExpr(expr->args[0].get());
            result.type = i32Type();
            result.int_value = (obj.string_value.find(search.string_value) != string::npos) ? 1 : 0;
        }
    } else if (expr->method_name == "startswith") {
        if (expr->args.size() >= 1) {
            Symbol prefix = evalExpr(expr->args[0].get());
            result.type = i32Type();
            result.int_value = (obj.string_value.find(prefix.string_value) == 0) ? 1 : 0;
        }
    } else if (expr->method_name == "endswith") {
        if (expr->args.size() >= 1) {
            Symbol suffix = evalExpr(expr->args[0].get());
            result.type = i32Type();
            if (obj.string_value.length() >= suffix.string_value.length()) {
                result.int_value = (obj.string_value.compare(
                    obj.string_value.length() - suffix.string_value.length(),
                    suffix.string_value.length(),
                    suffix.string_value) == 0) ? 1 : 0;
            } else {
                result.int_value = 0;
            }
        }
    }
    
    return result;
}