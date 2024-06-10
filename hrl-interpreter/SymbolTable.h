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
    unordered_map<string, vector<pair<string, string>>> struct_definitions;
    unordered_map<string, unordered_map<string, EvalResult>> struct_instances;

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

    void define_struct(const string& name, const vector<pair<string, string>>& fields) {
        struct_definitions[name] = fields;
    }

    void create_struct_instance(const string& instance_name, const string& struct_name) {
        if (struct_definitions.find(struct_name) == struct_definitions.end()) {
            throw runtime_error("Struct '" + struct_name + "' not defined.");
        }
        struct_instances[instance_name] = unordered_map<string, EvalResult>();
        for (const auto& field : struct_definitions[struct_name]) {
            struct_instances[instance_name][field.second] = {};
        }
    }

    unordered_map<string, EvalResult>* get_struct_instance(const string& instance_name) {
        auto iter = struct_instances.find(instance_name);
        if (iter != struct_instances.end()) {
            return &iter->second;
        }
        return nullptr;
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