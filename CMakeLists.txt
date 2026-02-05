cmake_minimum_required(VERSION 3.10)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(glc
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/evaluvator/evaluator.cpp
    src/semantics/semantic.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")