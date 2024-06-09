#include <string>
using namespace std;

class PrePro {
public:
    static string remove_comments(const string& input) {
        string filtered;
        bool in_comment = false;
        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == '-' && i + 1 < input.size() && input[i + 1] == '-') {
                in_comment = true;
                ++i;
            } else if (input[i] == '\n') {
                in_comment = false;
                filtered += input[i];
            } else if (!in_comment) {
                filtered += input[i];
            }
        }
        return filtered;
    }

    static string add_newline_eof(const string& input) {
        if (input.size() == 0 || input[input.size() - 1] != '\n') {
            return input + "\n";
        }
        return input;
    }

    static string remove_empty_lines(const string& input) {
        string filtered;
        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] != '\n' || (i + 1 < input.size() && input[i + 1] != '\n')) {
                filtered += input[i];
            }
        }
        return filtered;
    }

    static string preprocess(const string& input) {
        return add_newline_eof(remove_comments(remove_empty_lines(input)));
    }
};