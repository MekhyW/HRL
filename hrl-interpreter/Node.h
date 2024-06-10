#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <variant>
#include "SymbolTable.h"
using namespace std;

template <typename T>
string join(const vector<T>& vec, const string& delimiter) {
    stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) ss << delimiter;
        ss << vec[i];
    }
    return ss.str();
}

class Node;
using NodePtr = shared_ptr<Node>;
using EvalResult = variant<int, string, double, bool>;

class Node {
public:
    vector<NodePtr> statements;
    string type;
    static int i;
    int id;
    static int newId() { return ++i; }
    Node() : id(newId()) {}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const = 0;
    void add_statement(NodePtr statement) { statements.push_back(statement); }
};

int Node::i = 0;

class BinOpNode : public Node {
public:
    BinOpNode(string op, NodePtr left, NodePtr right) : op(op), left(move(left)), right(move(right)) {type = "BinOpNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        EvalResult right_value = right->Evaluate(symbol_table, func_table);
        EvalResult left_value = left->Evaluate(symbol_table, func_table);
        if (op != ".." && (holds_alternative<string>(left_value) != holds_alternative<string>(right_value))) {
            throw invalid_argument("Unsupported operation on string type");
        }
        if (op == "..") { 
            string left_str;
            string right_str;
            if (holds_alternative<int>(left_value)) { left_str = to_string(get<int>(left_value)); }
            else if (holds_alternative<double>(left_value)) { left_str = to_string(get<double>(left_value)); }
            else if (holds_alternative<string>(left_value)) { left_str = get<string>(left_value); }
            else if (holds_alternative<bool>(left_value)) { left_str = get<bool>(left_value) ? "1" : "0"; }
            if (holds_alternative<int>(right_value)) { right_str = to_string(get<int>(right_value)); }
            else if (holds_alternative<double>(right_value)) { right_str = to_string(get<double>(right_value)); }
            else if (holds_alternative<string>(right_value)) { right_str = get<string>(right_value); }
            else if (holds_alternative<bool>(right_value)) { right_str = get<bool>(right_value) ? "1" : "0"; }
            return EvalResult(left_str + right_str);
        }
        else if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
            int left_int;
            int right_int;
            if (holds_alternative<int>(left_value)) { left_int = get<int>(left_value); }
            else if (holds_alternative<double>(left_value)) { left_int = get<double>(left_value); }
            else if (holds_alternative<bool>(left_value)) { left_int = get<bool>(left_value); }
            if (holds_alternative<int>(right_value)) { right_int = get<int>(right_value); }
            else if (holds_alternative<double>(right_value)) { right_int = get<double>(right_value); }
            else if (holds_alternative<bool>(right_value)) { right_int = get<bool>(right_value); }
            if (op == "+") { return EvalResult(left_int + right_int); }
            else if (op == "-") { return EvalResult(left_int - right_int); }
            else if (op == "*") { return EvalResult(left_int * right_int); }
            else if (op == "/") {
                if (right_int == 0) { throw invalid_argument("Division by zero"); }
                return EvalResult(left_int / right_int);
            }
            else if (op == "%") {
                if (right_int == 0) { throw invalid_argument("Division by zero"); }
                return EvalResult(left_int % right_int);
            }
        }
        else if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=") { 
            int left_int;
            int right_int;
            if (holds_alternative<int>(left_value)) { left_int = get<int>(left_value); }
            else if (holds_alternative<double>(left_value)) { left_int = get<double>(left_value); }
            else if (holds_alternative<bool>(left_value)) { left_int = get<bool>(left_value); }
            if (holds_alternative<int>(right_value)) { right_int = get<int>(right_value); }
            else if (holds_alternative<double>(right_value)) { right_int = get<double>(right_value); }
            else if (holds_alternative<bool>(right_value)) { right_int = get<bool>(right_value); }
            if (holds_alternative<string>(left_value) && holds_alternative<string>(right_value)) {
                if (op == "==") { return EvalResult(get<string>(left_value) == get<string>(right_value)); }
                else if (op == "!=") { return EvalResult(get<string>(left_value) != get<string>(right_value)); }
                else if (op == "<") { return EvalResult(get<string>(left_value) < get<string>(right_value)); }
                else if (op == "<=") { return EvalResult(get<string>(left_value) <= get<string>(right_value)); }
                else if (op == ">") { return EvalResult(get<string>(left_value) > get<string>(right_value)); }
                else if (op == ">=") { return EvalResult(get<string>(left_value) >= get<string>(right_value)); }
                else { throw invalid_argument("Invalid operation on string type"); }
            }
            if (op == "==") { return EvalResult(left_int == right_int); }
            else if (op == "!=") { return EvalResult(left_int != right_int); }
            else if (op == "<") { return EvalResult(left_int < right_int); }
            else if (op == "<=") { return EvalResult(left_int <= right_int); }
            else if (op == ">") { return EvalResult(left_int > right_int); }
            else if (op == ">=") { return EvalResult(left_int >= right_int); }
            else if (op == "and") { return EvalResult(left_int && right_int); }
            else if (op == "or") { return EvalResult(left_int || right_int); }
        }
        else if (op == "and" || op == "or") {
            bool left_bool;
            bool right_bool;
            if (holds_alternative<int>(left_value)) { left_bool = get<int>(left_value) != 0; }
            else if (holds_alternative<double>(left_value)) { left_bool = get<double>(left_value) != 0; }
            else if (holds_alternative<bool>(left_value)) { left_bool = get<bool>(left_value); }
            if (holds_alternative<int>(right_value)) { right_bool = get<int>(right_value) != 0; }
            else if (holds_alternative<double>(right_value)) { right_bool = get<double>(right_value) != 0; }
            else if (holds_alternative<bool>(right_value)) { right_bool = get<bool>(right_value); }
            if (op == "and") { return EvalResult(left_bool && right_bool); }
            else if (op == "or") { return EvalResult(left_bool || right_bool); }
        }
        else { throw invalid_argument("Invalid binary operation"); }
        return EvalResult(0);
    }
private:
    string op;
    NodePtr left, right;
};

class UnOpNode : public Node {
public:
    UnOpNode(string op, NodePtr child) : op(op), child(move(child)) {type = "UnOpNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        EvalResult child_value = child->Evaluate(symbol_table, func_table);
        if (op == "+") { return get<int>(child_value); }
        else if (op == "-") { return -get<int>(child_value); }
        else if (op == "not") { return !get<bool>(child_value); }
        else { throw invalid_argument("Invalid unary operation"); }
    }
private:
    string op;
    NodePtr child;
};

class NoOpNode : public Node {
public:
    NoOpNode() {type = "NoOpNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override { 
        return EvalResult("NULL"); 
    }
};

class IntValNode : public Node {
public:
    IntValNode(int val) : value(val) {type = "IntValNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        return EvalResult(value);
    }
private:
    int value;
};

class StringValNode : public Node {
public:
    StringValNode(string val) : value(val) {type = "StringValNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override { 
        return EvalResult(value);
    }
private:
    string value;
};

class VarNode : public Node {
public:
    VarNode(string identifier) : identifier(identifier) {type = "VarNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override { 
        return symbol_table.getVariable(identifier); 
    }
private:
    string identifier;
};

class ReadNode : public Node {
public:
    ReadNode() {type = "ReadNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        int value;
        cin >> value;
        return EvalResult(value);
    }
};

class PrintNode : public Node {
public:
    PrintNode(NodePtr expression) : expression(move(expression)) {type = "PrintNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table);
        if (holds_alternative<int>(result)) { cout << get<int>(result) << endl; }
        else if (holds_alternative<string>(result)) { cout << get<string>(result) << endl; }
        else if (holds_alternative<double>(result)) { cout << get<double>(result) << endl; }
        else if (holds_alternative<bool>(result)) { cout << get<bool>(result) << endl; }
        return result;
    }
private:
    NodePtr expression;
};

class CallProgramNode : public Node {
public:
    CallProgramNode(NodePtr program_name_expression, const vector<NodePtr>& args) : program_name_expression(move(program_name_expression)), args(args) {type = "CallProgramNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        string program_name = get<string>(program_name_expression->Evaluate(symbol_table, func_table));
        vector<string> args_strings;
        for (NodePtr arg : args) { args_strings.push_back(get<string>(arg->Evaluate(symbol_table, func_table))); }
        if (program_name.find(".py") != string::npos) {
            string command = "python " + program_name + " " + join(args_strings, " ");
            return EvalResult(system(command.c_str()));
        }
        else if (program_name.find(".lua") != string::npos) {
            string command = "lua " + program_name + " " + join(args_strings, " ");
            return EvalResult(system(command.c_str()));
        }
        else {
            string command = "./" + program_name + " " + join(args_strings, " ");
            return EvalResult(system(command.c_str()));
        }
    }
private:
    NodePtr program_name_expression;
    vector<NodePtr> args;
};

class VarDeclareNode : public Node {
public:
    VarDeclareNode(string identifier, NodePtr expression = make_shared<IntValNode>(0))
        : identifier(identifier), expression(move(expression)) {type = "VarDeclareNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table);
        if (result == EvalResult("NULL")) { throw invalid_argument("Cannot assign NULL value to variable " + identifier); }
        symbol_table.setVariable(identifier, result, true);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class AssignmentNode : public Node {
public:
    AssignmentNode(string identifier, NodePtr expression) : identifier(identifier), expression(move(expression)) {type = "AssignmentNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table);
        if (result == EvalResult("NULL")) { throw invalid_argument("Cannot assign NULL value to variable " + identifier); }
        symbol_table.setVariable(identifier, result, false);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class WhileNode : public Node {
public:
    WhileNode(NodePtr condition, NodePtr block) : condition(move(condition)), block(move(block)) {type = "WhileNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        while (get<bool>(condition->Evaluate(symbol_table, func_table))) { block->Evaluate(symbol_table, func_table); }
        return EvalResult("NULL");
    }
private:
    NodePtr condition, block;
};

class IfNode : public Node {
public:
    IfNode(NodePtr condition, NodePtr block, NodePtr else_block) : condition(move(condition)), block(move(block)), else_block(move(else_block)) {type = "IfNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        EvalResult result = condition->Evaluate(symbol_table, func_table);
        if (get<bool>(result)) { return block->Evaluate(symbol_table, func_table); }
        else { return else_block->Evaluate(symbol_table, func_table); }
    }
private:
    NodePtr condition, block, else_block;
};

class FuncDeclareNode : public Node {
public:
    FuncDeclareNode(const string& func_name, const vector<string>& args, NodePtr block_node)
        : func_name(func_name), args(args), block_node(move(block_node)) {type = "FuncDeclareNode";}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        func_table.setFunction(func_name, args, block_node);
        return EvalResult("NULL");
    }
private:
    string func_name;
    vector<string> args;
    NodePtr block_node;
};

class FuncCallNode : public Node {
public:
    FuncCallNode(const string& identifier, const vector<NodePtr>& args) : identifier(identifier), args(args) {type = "FuncCallNode";}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        if (identifier == "print") { return make_shared<PrintNode>(args[0])->Evaluate(symbol_table, func_table); }
        if (identifier == "read") { return make_shared<ReadNode>()->Evaluate(symbol_table, func_table); }
        if (identifier == "callprogram") { return make_shared<CallProgramNode>(args[0], vector<NodePtr>(args.begin() + 1, args.end()))->Evaluate(symbol_table, func_table); }
        FuncInfo func_info = func_table.getFunction(identifier);
        if (func_info.args.size() != args.size()) { throw invalid_argument("Function " + identifier + " expects " + to_string(func_info.args.size()) + " arguments, but " + to_string(args.size()) + " were given"); }
        SymbolTable new_symbol_table = SymbolTable();
        for (int i = 0; i < func_info.args.size(); i++) { new_symbol_table.setVariable(func_info.args[i], args[i]->Evaluate(symbol_table, func_table), true); }
        return func_info.block->Evaluate(new_symbol_table, func_table);
    }
private:
    string identifier;
    vector<NodePtr> args;
};

class ReturnNode : public Node {
public:
    ReturnNode(NodePtr return_node) : return_node(move(return_node)) {type = "ReturnNode";}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        return return_node->Evaluate(symbol_table, func_table);
    }
private:
    NodePtr return_node;
};

class BlockNode : public Node {
public:
    BlockNode() {type = "BlockNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        for (const auto& statement : statements) {
            if (statement->type == "ReturnNode") { return statement->Evaluate(symbol_table, func_table); }
            else { statement->Evaluate(symbol_table, func_table); }
        }
        return EvalResult("NULL");
    }
};

class EnumNode : public Node {
public:
    EnumNode(string name, vector<string> values) : name(name), values(values) { type = "EnumNode"; }
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        symbol_table.setEnum(name, values);
        return EvalResult("NULL");
    }
private:
    string name;
    vector<string> values;
};

class EnumValNode : public Node {
public:
    EnumValNode(string enumName, string valueName) : enumName(enumName), valueName(valueName) { type = "EnumValNode"; }
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table) const override {
        int value = symbol_table.getEnumValue(enumName, valueName);
        return EvalResult(value);
    }
private:
    string enumName;
    string valueName;
};
