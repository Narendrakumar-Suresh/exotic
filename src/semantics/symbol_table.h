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
    vector<Symbol> list_values;
};

class SymbolTable {
public:
    void set(const string& name, const Symbol& sym){
        table[name] = sym;
    }
    
    Symbol get(const string& name){
        if (table.find(name) == table.end()) {
            Symbol empty;
            empty.type = i32Type();
            empty.int_value = 0;
            return empty;
        }
        return table[name];
    }
    
    bool exists(const string& name){
        return table.find(name) != table.end();
    }

private:
    unordered_map<string, Symbol> table;
};

#endif