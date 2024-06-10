#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <variant>
using namespace std;

class Node;
using NodePtr = shared_ptr<Node>;
using EvalResult = std::variant<int, string, double, bool>;

struct FuncInfo {
    vector<string> args;
    NodePtr block;
};

class SymbolTable {
private:
    unordered_map<string, EvalResult> variables;
    unordered_map<string, unordered_map<string, int>> enums;

public:
    void setVariable(const string& name, EvalResult value, bool declare = false) {
        variables[name] = value;
    }

    EvalResult getVariable(const string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) { return it->second; }
        else { throw invalid_argument("Undefined variable: " + name); }
    }

    void setEnum(const string& name, const vector<string>& values) {
        unordered_map<string, int> enumValues;
        for (int i = 0; i < values.size(); ++i) {
            enumValues[values[i]] = i;
        }
        enums[name] = enumValues;
    }
    
    int getEnumValue(const string& enumName, const string& valueName) {
        auto it = enums.find(enumName);
        if (it != enums.end() && it->second.find(valueName) != it->second.end()) {
            return it->second[valueName];
        } else {
            throw invalid_argument("Undefined enum or value: " + enumName + "::" + valueName);
        }
    }
};

class FuncTable {
private:
    unordered_map<string, FuncInfo> functions;

public:
    void setFunction(const string& name, const vector<string>& args, const NodePtr& block) {
        if (functions.find(name) != functions.end()) { throw invalid_argument("Function already declared: " + name); }
        functions[name] = {args, block};
    }

    FuncInfo getFunction(const string& name) {
        auto it = functions.find(name);
        if (it != functions.end()) { return it->second; } 
        else { throw invalid_argument("Undefined function: " + name); }
    }
};