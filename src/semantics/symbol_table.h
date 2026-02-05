#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
using namespace std;

struct Symbol{
    string name;
    int int_value;
    double double_value;
    string string_value;
}

class SymbolTable {
    public:
    void set(const string& name, int value){
        table[name] = value;
    }
    int get (const string& name){
        return table[name];
    }

    private:
    std::unordered_map<string, Symbol> table;
};

#endif