#include <iostream>
#include "lexer/lexer.h"
#include "parser/parser.h"

int main() {
    std::string code =
        "let x = 1;\n"
        "let y = x + 2;\n"
        "let z = x + y;\n";

    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parseProgram();

    for (auto& stmt : program) {
        std::cout << stmt.name << " = " << stmt.expr.value << "\n";
    }
}
