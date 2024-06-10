#include <iostream>
#include <string>
#include "Tokenizer.h"
#include "Node.h"

using namespace std;

class Parser {
private:
    static Token current_token;

public:
    static Tokenizer tokenizer;

    shared_ptr<Node> parse_program() {
        if (current_token.type != "SETUP") { throw invalid_argument("Program must start with 'setup'"); }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> setup_block = parse_block();
        if (current_token.type != "MAIN") { throw invalid_argument("Missing main block after setup"); }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> main_block = parse_block();
        return make_shared<ProgramNode>(setup_block, main_block);
    }

    shared_ptr<Node> parse_block() {
        shared_ptr<Node> block_node = make_shared<BlockNode>();
        if (current_token.type != "LBRACE") { throw invalid_argument("Expected '{' at start of block"); }
        current_token = tokenizer.selectNext();
        while (current_token.type != "EOF" && current_token.type != "RBRACE" && current_token.type != "ELSE") {
            block_node->add_statement(parse_statement());
        }
        if (current_token.type != "RBRACE") { throw invalid_argument("Expected '}' at end of block"); }
        current_token = tokenizer.selectNext();
        return block_node;
    }

    shared_ptr<Node> parse_statement() {
        if (current_token.type == "EOF") {
            return make_shared<NoOpNode>();
        } else if (current_token.type == "NEWLINE" || current_token.type == "RBRACE" || current_token.type == "ELSE") {
            current_token = tokenizer.selectNext();
            return make_shared<NoOpNode>();
        } else if (current_token.type == "CONST") {
            return parse_const_declaration();
        } else if (current_token.type == "IF") {
            return parse_if_statement();
        } else if (current_token.type == "WHILE") {
            return parse_while_statement();
        } else if (current_token.type == "RETURN") {
            return parse_return_statement();
        } else if (current_token.type == "FUNCTION") {
            return parse_function_declaration();
        } else if (current_token.type == "ENUM") {
            return parse_enum_declaration();
        } else if (current_token.type == "STRUCT") {
            return parse_struct_declaration();
        } else if (current_token.type == "THREADLOOP") {
            return parse_threadloop_statement();
        } else {
            return parse_vardec_assignment_funccall();
        }
    }

    shared_ptr<Node> parse_threadloop_statement() {
        current_token = tokenizer.selectNext();
        if (current_token.type != "IDENTIFIER") { throw invalid_argument("Expected identifier after 'threadloop'"); }
        string threadloop_name = current_token.valueString;
        current_token = tokenizer.selectNext();
        if (current_token.type != "LPAREN") { throw invalid_argument("Expected '(' after threadloop name"); }
        current_token = tokenizer.selectNext();
        vector<string> args;
        while (current_token.type != "RPAREN") {
            if (current_token.type != "IDENTIFIER") { throw invalid_argument("Expected identifier in threadloop arguments"); }
            args.push_back(current_token.valueString);
            current_token = tokenizer.selectNext();
            if (current_token.type == "RPAREN") { break; }
            if (current_token.type != "COMMA") { throw invalid_argument("Expected ',' after threadloop argument"); }
            current_token = tokenizer.selectNext();
        }
        current_token = tokenizer.selectNext();
        if (current_token.type != "LBRACE") { throw invalid_argument("Expected '{' after threadloop arguments"); }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> block_node = parse_block();
        if (current_token.type != "RBRACE") { throw invalid_argument("Expected '}' after threadloop block"); }
        current_token = tokenizer.selectNext();
        return make_shared<ThreadLoopNode>(threadloop_name, args, block_node);
    }

    shared_ptr<Node> parse_enum_declaration() {
        current_token = tokenizer.selectNext();
        if (current_token.type != "IDENTIFIER") {
            throw invalid_argument("Expected identifier after 'enum'");
        }
        string enum_name = current_token.valueString;
        current_token = tokenizer.selectNext();
        if (current_token.type != "LBRACE") {
            throw invalid_argument("Expected '{' after enum name");
        }
        current_token = tokenizer.selectNext();
        vector<string> values;
        while (current_token.type != "RBRACE") {
            if (current_token.type != "IDENTIFIER") {
                throw invalid_argument("Expected identifier in enum values");
            }
            values.push_back(current_token.valueString);
            current_token = tokenizer.selectNext();
            if (current_token.type == "COMMA") {
                current_token = tokenizer.selectNext();
            }
        }
        current_token = tokenizer.selectNext();
        return make_shared<EnumNode>(enum_name, values);
    }

    shared_ptr<Node> parse_struct_declaration() {
        current_token = tokenizer.selectNext();
        if (current_token.type != "IDENTIFIER") {
            throw invalid_argument("Expected identifier after 'struct'");
        }
        string struct_name = current_token.valueString;
        current_token = tokenizer.selectNext();
        if (current_token.type != "LBRACE") {
            throw invalid_argument("Expected '{' after struct name");
        }
        current_token = tokenizer.selectNext();
        vector<pair<string, string>> fields;
        while (current_token.type != "RBRACE") {
            if (current_token.type != "IDENTIFIER") {
                throw invalid_argument("Expected type identifier in struct field declaration");
            }
            string field_type = current_token.valueString;
            current_token = tokenizer.selectNext();
            if (current_token.type != "IDENTIFIER") {
                throw invalid_argument("Expected field name in struct field declaration");
            }
            string field_name = current_token.valueString;
            fields.emplace_back(field_type, field_name);

            current_token = tokenizer.selectNext();
            if (current_token.type == "COMMA") {
                current_token = tokenizer.selectNext();
            } else if (current_token.type != "RBRACE") {
                throw invalid_argument("Expected '}' or ',' in struct declaration");
            }
        }
        current_token = tokenizer.selectNext();
        return make_shared<StructNode>(struct_name, fields);
    }

    shared_ptr<Node> parse_const_declaration() {
        current_token = tokenizer.selectNext();
        if (current_token.type != "IDENTIFIER") {
            throw invalid_argument("Expected identifier after 'const'");
        }
        string var_name = current_token.valueString;
        current_token = tokenizer.selectNext();
        if (current_token.type != "ASSIGN") {
            throw invalid_argument("Expected '=' after identifier in constant declaration");
        }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> value_node = parse_expression_or_list();
        if (current_token.type != "SEMICOLON") {
            throw invalid_argument("Expected ';' after constant declaration");
        }
        current_token = tokenizer.selectNext();
        return make_shared<VarDeclareNode>(var_name, value_node);
    }

    shared_ptr<Node> parse_expression_or_list() {
        if (current_token.type == "LBRACKET") {
            return parse_list_initializer();
        } else {
            return parse_boolexpression();
        }
    }

    shared_ptr<Node> parse_list_initializer() {
        current_token = tokenizer.selectNext();
        vector<shared_ptr<Node>> elements;
        while (current_token.type != "RBRACKET") {
            shared_ptr<Node> element = parse_expression();
            elements.push_back(element);
            if (current_token.type == "COMMA") {
                current_token = tokenizer.selectNext();
            } else if (current_token.type != "RBRACKET") {
                throw invalid_argument("Expected ']' or ',' in list initializer");
            }
        }
        current_token = tokenizer.selectNext();
        return make_shared<ArrayNode>(elements);
    }

    shared_ptr<Node> parse_if_statement() {
        current_token = tokenizer.selectNext();
        shared_ptr<Node> condition = parse_boolexpression();
        if (current_token.type != "LBRACE") {
            throw invalid_argument("Expected '{' after if condition");
        }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> if_block = parse_block();
        if (current_token.type != "RBRACE") {
            throw invalid_argument("Expected '}' after if block");
        }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> else_block = make_shared<NoOpNode>();
        if (current_token.type == "ELSE") {
            current_token = tokenizer.selectNext();
            if (current_token.type != "LBRACE") {
                throw invalid_argument("Expected '{' after else");
            }
            current_token = tokenizer.selectNext();
            else_block = parse_block();
            if (current_token.type != "RBRACE") {
                throw invalid_argument("Expected '}' after else block");
            }
            current_token = tokenizer.selectNext();
        }
        return make_shared<IfNode>(condition, if_block, else_block);
    }

    shared_ptr<Node> parse_while_statement() {
        current_token = tokenizer.selectNext();
        shared_ptr<Node> condition = parse_boolexpression();
        if (current_token.type != "LBRACE") {
            throw invalid_argument("Expected '{' after while condition");
        }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> block_node = parse_block();
        if (current_token.type != "RBRACE") {
            throw invalid_argument("Expected '}' after while block");
        }
        current_token = tokenizer.selectNext();
        return make_shared<WhileNode>(condition, block_node);
    }

    shared_ptr<Node> parse_return_statement() {
        current_token = tokenizer.selectNext();
        shared_ptr<Node> return_node = parse_boolexpression();
        if (current_token.type != "SEMICOLON") {
            throw invalid_argument("Expected ';' after return statement");
        }
        current_token = tokenizer.selectNext();
        return make_shared<ReturnNode>(return_node);
    }

    shared_ptr<Node> parse_function_declaration() {
        current_token = tokenizer.selectNext();
        if (current_token.type != "IDENTIFIER") {
            throw invalid_argument("Expected identifier after 'function'");
        }
        string func_name = current_token.valueString;
        current_token = tokenizer.selectNext();
        if (current_token.type != "LPAREN") {
            throw invalid_argument("Expected '(' after function name");
        }
        current_token = tokenizer.selectNext();
        vector<string> args;
        while (current_token.type != "RPAREN") {
            if (current_token.type != "IDENTIFIER") {
                throw invalid_argument("Expected identifier in function arguments");
            }
            args.push_back(current_token.valueString);
            current_token = tokenizer.selectNext();
            if (current_token.type == "RPAREN") {
                break;
            }
            if (current_token.type != "COMMA") {
                throw invalid_argument("Expected ',' after function argument");
            }
            current_token = tokenizer.selectNext();
        }
        current_token = tokenizer.selectNext();
        if (current_token.type != "LBRACE") {
            throw invalid_argument("Expected '{' after function arguments");
        }
        current_token = tokenizer.selectNext();
        shared_ptr<Node> block_node = parse_block();
        if (current_token.type != "RBRACE") {
            throw invalid_argument("Expected '}' after function block");
        }
        current_token = tokenizer.selectNext();
        return make_shared<FuncDeclareNode>(func_name, args, block_node);
    }

    shared_ptr<Node> parse_vardec_assignment_funccall() {
        if (current_token.type != "IDENTIFIER") {
            throw invalid_argument("Expected identifier");
        }
        string identifier = current_token.valueString;
        current_token = tokenizer.selectNext();
        if (current_token.type == "ASSIGN") {
            current_token = tokenizer.selectNext();
            shared_ptr<Node> value_node = parse_boolexpression();
            if (current_token.type != "SEMICOLON") {
                throw invalid_argument("Expected ';' after assignment");
            }
            current_token = tokenizer.selectNext();
            return make_shared<AssignmentNode>(identifier, value_node);
        } else if (current_token.type == "LPAREN") {
            current_token = tokenizer.selectNext();
            vector<shared_ptr<Node>> args;
            while (current_token.type != "RPAREN") {
                args.push_back(parse_boolexpression());
                if (current_token.type == "RPAREN") {
                    break;
                }
                if (current_token.type != "COMMA") {
                    throw invalid_argument("Expected ',' after function argument");
                }
                current_token = tokenizer.selectNext();
            }
            current_token = tokenizer.selectNext();
            if (current_token.type != "SEMICOLON") {
                throw invalid_argument("Expected ';' after function call");
            }
            current_token = tokenizer.selectNext();
            return make_shared<FuncCallNode>(identifier, args);
        } else if (current_token.type == "COLON") {
            current_token = tokenizer.selectNext();
            if (current_token.type != "IDENTIFIER") {
                throw invalid_argument("Expected type identifier after ':' in variable declaration");
            }
            current_token = tokenizer.selectNext();
            shared_ptr<Node> value_node;
            if (current_token.type == "ASSIGN") {
                current_token = tokenizer.selectNext();
                value_node = parse_expression_or_list();
            }
            if (current_token.type != "SEMICOLON") {
                throw invalid_argument("Expected ';' after variable declaration");
            }
            current_token = tokenizer.selectNext();
            return make_shared<VarDeclareNode>(identifier, value_node);
        }
        else {
            throw invalid_argument("Unexpected token after identifier");
        }
    }

    shared_ptr<Node> parse_boolexpression() {
        shared_ptr<Node> bool_expression_node = parse_boolterm();
        while (current_token.type == "OR") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_bool_term_node = parse_boolterm();
            bool_expression_node = make_shared<BinOpNode>("or", bool_expression_node, next_bool_term_node);
        }
        return bool_expression_node;
    }

    shared_ptr<Node> parse_boolterm() {
        shared_ptr<Node> bool_term_node = parse_relexpression();
        while (current_token.type == "AND") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_rel_expression_node = parse_relexpression();
            bool_term_node = make_shared<BinOpNode>("and", bool_term_node, next_rel_expression_node);
        }
        return bool_term_node;
    }

    shared_ptr<Node> parse_relexpression() {
        shared_ptr<Node> relexpression_node = parse_expression();
        while (current_token.type == "EQ" || current_token.type == "NEQ" 
        || current_token.type == "GT" || current_token.type == "LT" || current_token.type == "GE" || current_token.type == "LE") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_expression_node = parse_expression();
            if (op_token.type == "GT") { relexpression_node = make_shared<BinOpNode>(">", relexpression_node, next_expression_node); }
            else if (op_token.type == "LT") { relexpression_node = make_shared<BinOpNode>("<", relexpression_node, next_expression_node); }
            else if (op_token.type == "GE") { relexpression_node = make_shared<BinOpNode>(">=", relexpression_node, next_expression_node); }
            else if (op_token.type == "LE") { relexpression_node = make_shared<BinOpNode>("<=", relexpression_node, next_expression_node); }
            else if (op_token.type == "EQ") { relexpression_node = make_shared<BinOpNode>("==", relexpression_node, next_expression_node); }
            else { relexpression_node = make_shared<BinOpNode>("!=", relexpression_node, next_expression_node); }
        }
        return relexpression_node;
    }

    shared_ptr<Node> parse_expression() {
        shared_ptr<Node> expression_node = parse_term();
        while (current_token.type == "PLUS" || current_token.type == "MINUS" || current_token.type == "CONCAT") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_term_node = parse_term();
            if (op_token.type == "PLUS") { expression_node = make_shared<BinOpNode>("+", expression_node, next_term_node); }
            else if (op_token.type == "MINUS") { expression_node = make_shared<BinOpNode>("-", expression_node, next_term_node); } 
            else if (op_token.type == "CONCAT") { expression_node = make_shared<BinOpNode>("..", expression_node, next_term_node); }
        }
        return expression_node;
    }

    shared_ptr<Node> parse_term() {
        shared_ptr<Node> term_node = parse_factor();
        while (current_token.type == "MULT" || current_token.type == "DIV" || current_token.type == "MOD") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_factor_node = parse_factor();
            if (op_token.type == "MULT") { term_node = make_shared<BinOpNode>("*", term_node, next_factor_node); }
            else if (op_token.type == "DIV") { term_node = make_shared<BinOpNode>("/", term_node, next_factor_node); }
            else { term_node = make_shared<BinOpNode>("%", term_node, next_factor_node); }
        }
        return term_node;
    }

    shared_ptr<Node> parse_factor() {
        if (current_token.type == "NUMBER_LITERAL") {
            int value = current_token.value;
            current_token = tokenizer.selectNext();
            return make_shared<IntValNode>(value);
        }
        else if (current_token.type == "STRING_LITERAL") {
            string value = current_token.valueString;
            current_token = tokenizer.selectNext();
            return make_shared<StringValNode>(value);
        }
        else if (current_token.type == "LPAREN") {
            current_token = tokenizer.selectNext();
            shared_ptr<Node> bool_expression_node = parse_boolexpression();
            if (current_token.type != "RPAREN") { throw invalid_argument("Expected ')' after expression"); }
            current_token = tokenizer.selectNext();
            return bool_expression_node;
        }
        else if (current_token.type == "MINUS") {
            current_token = tokenizer.selectNext();
            return make_shared<BinOpNode>("-", make_shared<IntValNode>(0), parse_factor());
        }
        else if (current_token.type == "PLUS") {
            current_token = tokenizer.selectNext();
            return parse_factor();
        }
        else if (current_token.type == "NOT") {
            current_token = tokenizer.selectNext();
            return make_shared<UnOpNode>("!", parse_factor());
        }
        else if (current_token.type == "IDENTIFIER") {
            string identifier = current_token.valueString;
            current_token = tokenizer.selectNext();
            if (current_token.type == "DOT") {
                current_token = tokenizer.selectNext();
                if (current_token.type != "IDENTIFIER") { throw invalid_argument("Expected field name after '.'"); }
                string field_name = current_token.valueString;
                current_token = tokenizer.selectNext();
                return make_shared<StructFieldNode>(identifier, field_name);
            }
            else if (current_token.type == "LPAREN") {
                current_token = tokenizer.selectNext();
                vector<shared_ptr<Node>> args;
                while (current_token.type != "RPAREN") {
                    args.push_back(parse_boolexpression());
                    if (current_token.type == "RPAREN") { break; }
                    if (current_token.type != "COMMA") { throw invalid_argument("Expected ',' after function argument"); }
                    current_token = tokenizer.selectNext();
                }
                current_token = tokenizer.selectNext();
                return make_shared<FuncCallNode>(identifier, args);
            }
            else if (current_token.type == "COLON") {
                current_token = tokenizer.selectNext();
                if (current_token.type != "IDENTIFIER") {
                    throw invalid_argument("Expected enum value after '::'");
                }
                string enum_value = current_token.valueString;
                current_token = tokenizer.selectNext();
                return make_shared<EnumValNode>(identifier, enum_value);
            }
            else if (current_token.type == "LBRACKET") {
                current_token = tokenizer.selectNext();
                shared_ptr<Node> index = parse_boolexpression();
                if (current_token.type != "RBRACKET") { throw invalid_argument("Expected ']' after array index"); }
                current_token = tokenizer.selectNext();
                return make_shared<ArrayAccessNode>(identifier, index);
            }
            else {
                return make_shared<VarNode>(identifier);
            }
        }
        throw invalid_argument("Unexpected token: " + current_token.type);
    }

    shared_ptr<Node> run(const string& code) {
        current_token = tokenizer.selectNext();
        shared_ptr<Node> root = parse_program();
        if (current_token.type != "EOF") { throw invalid_argument("Expected EOF"); }
        return root;
    }
};