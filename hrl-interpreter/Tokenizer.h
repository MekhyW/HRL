#include <iostream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <stdexcept>
using namespace std;

class Token {
public:
    string type;
    int value;
    string valueString;
};

class Tokenizer {
private:
    string source;
    size_t position;
    Token next;

public:
    unordered_map<string, string> keywordMap;
    Tokenizer(string src) : source(src), position(0), next({ "", 0 }) {
        keywordMap["setup"] = "SETUP";
        keywordMap["main"] = "MAIN";
        keywordMap["enum"] = "ENUM";
        keywordMap["struct"] = "STRUCT";
        keywordMap["behavior"] = "BEHAVIOR";
        keywordMap["loop"] = "LOOP";
        keywordMap["if"] = "IF";
        keywordMap["else"] = "ELSE";
        keywordMap["switch"] = "SWITCH";
        keywordMap["case"] = "CASE";
        keywordMap["const"] = "CONST";
        keywordMap["function"] = "FUNCTION";
        keywordMap["threadloop"] = "THREADLOOP";
        keywordMap["break"] = "BREAK";
        keywordMap["continue"] = "CONTINUE";
        keywordMap["return"] = "RETURN";
        keywordMap["=="] = "EQ";
        keywordMap["!="] = "NEQ";
        keywordMap["<"] = "LT";
        keywordMap["<="] = "LE";
        keywordMap[">"] = "GT";
        keywordMap[">="] = "GE";
        keywordMap[":"] = "COLON";
        keywordMap[","] = "COMMA";
        keywordMap["="] = "ASSIGN";
        keywordMap[";"] = "SEMICOLON";
        keywordMap["{"] = "LBRACE";
        keywordMap["}"] = "RBRACE";
        keywordMap["("] = "LPAREN";
        keywordMap[")"] = "RPAREN";
        keywordMap["."] = "DOT";
        keywordMap["["] = "LBRACKET";
        keywordMap["]"] = "RBRACKET";
        keywordMap["+"] = "PLUS";
        keywordMap["-"] = "MINUS";
        keywordMap["*"] = "MULT";
        keywordMap["/"] = "DIV";
        keywordMap["%"] = "MOD";
        keywordMap["&&"] = "AND";
        keywordMap["||"] = "OR";
        keywordMap["!"] = "NOT";
        keywordMap[".."] = "CONCAT";
    }

    void updateNextToken() {
        if (source.empty()) {
            throw invalid_argument("Empty Input");
        }
        while (position < source.size()) {
            char current_char = source[position];
            if (isspace(current_char)) {
                position++;
            } else if (isdigit(current_char)) {
                next.type = "NUMBER_LITERAL";
                next.value = current_char - '0';
                position++;
                while (position < source.size() && isdigit(source[position])) {
                    next.value = next.value * 10 + (source[position] - '0');
                    position++;
                }
                return;
            } else if (isalpha(current_char) || current_char == '_') {
                string identifier = "";
                identifier += current_char;
                position++;
                while (position < source.size() && (isalnum(source[position]) || source[position] == '_')) {
                    identifier += source[position];
                    position++;
                }
                if (keywordMap.find(identifier) != keywordMap.end()) {
                    next.type = keywordMap[identifier];
                } else {
                    next.type = "IDENTIFIER";
                    next.valueString = identifier;
                }
                return;
            } else if (current_char == '"') {
                next.type = "STRING_LITERAL";
                next.valueString = "";
                position++;
                while (position < source.size() && source[position] != '"') {
                    if (source[position] == '\\' && position + 1 < source.size() && source[position + 1] == '"') {
                        next.valueString += '"';
                        position += 2;
                    } else {
                        next.valueString += source[position];
                        position++;
                    }
                }
                if (position >= source.size() || source[position] != '"') {
                    throw invalid_argument("Unterminated string");
                }
                position++;
                return;
            } else {
                string identifier = "";
                identifier += current_char;
                position++;
                if ((current_char == '=' || current_char == '!' || current_char == '<' || current_char == '>') &&
                    position < source.size() && source[position] == '=') {
                    identifier += source[position];
                    position++;
                } else if (current_char == '&' && position < source.size() && source[position] == '&') {
                    identifier += source[position];
                    position++;
                } else if (current_char == '|' && position < source.size() && source[position] == '|') {
                    identifier += source[position];
                    position++;
                } else if (current_char == '.' && position < source.size() && source[position] == '.') {
                    identifier += source[position];
                    position++;
                }
                if (keywordMap.find(identifier) != keywordMap.end()) {
                    next.type = keywordMap[identifier];
                } else {
                    throw invalid_argument("Invalid operator: '" + identifier + "'");
                }
                return;
            }
        }
        next.type = "EOF";
        next.value = 0;
    }

    Token selectNext() {
        updateNextToken();
        return next;
    }
};
