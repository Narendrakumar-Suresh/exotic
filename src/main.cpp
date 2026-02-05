#include "lexer/lexer.h"
#include "parser/parser.h"
#include "evaluvator/evaluator.h"
#include "semantics/symbol_table.h"
#include "semantics/semantic.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    string filename = argv[1];
    if (filename.length() < 3 || filename.substr(filename.length() - 2) != ".g") {
        cerr << "Error: Input file must have a .g extension.\n";
        return 1;
    }
    
    ifstream file(argv[1]);
    if (!file) {
        cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    string source = buffer.str();
    
    Lexer lexer(source);
    Parser parser(lexer);
    
    vector<unique_ptr<Stmt>> program = parser.parseProgram();
    
    // Semantic Analysis
    SymbolTable semanticSymbols;
    SemanticAnalyzer semanticAnalyzer(semanticSymbols);
    semanticAnalyzer.analyze(program);
    
    // Evaluation
    SymbolTable evaluatorSymbols;
    Evaluator evaluator(evaluatorSymbols);
    evaluator.evalProgram(program);
    
    cout << "Program executed successfully\n";
    
    return 0;
}