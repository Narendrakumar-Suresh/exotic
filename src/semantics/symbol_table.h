#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include "types.h"
using namespace std;

struct Symbol{
    string name;
    Type type;
    int int_value;
    double double_value;
    string string_value;
};

class SymbolTable {
public:
    void set(const string& name, int value){
        Symbol sym;
        sym.name = name;
        sym.int_value = value;
        sym.type = i32Type();
        table[name] = sym;
    }
    
    int get(const string& name){
        return table[name].int_value;
    }

private:
    std::unordered_map<string, Symbol> table;
};

#endif