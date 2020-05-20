#include "args.hpp"
#include <algorithm>

using std::string;
using std::vector;
using std::find_if;

string trim(const string &source, const char *delims)
{
    string result(source);
    size_t index = result.find_last_not_of(delims);
    if (index != string::npos)
        result.erase(++index);

    index = result.find_first_not_of(delims);
    if (index != string::npos)
        result.erase(0, index);
    else
        result.erase();
    return result;
}

vector<string> split(const string &s)
{
    vector<string> result;
    auto it = s.begin();
    while (it != s.end()) {
        it = find_if(it, s.end(), [](char c) {
            return c != ' ';
        });
        auto jt = find_if(it, s.end(), [](char c) {
            return c == ' ';
        });
        if (it != s.end())
            result.push_back(string(it, jt));
        it = jt;
    }
    return result;
}

vector<string> parse_argv(int argc, const char *argv[])
{
    vector<string> args(argv, argv + argc);
    vector<string> opts(10);
    size_t pos_equal;
    string opt, value;
    for (auto &a : args) {
        pos_equal = a.find("=");
        if (pos_equal != a.npos) {
            opt = trim(a.substr(0, pos_equal));
            value = trim(a.substr(pos_equal + 1));
            if (opt == "--show_hidden")
                opts[INDEX_ARG_HIDDEN_FILES] = value;
            else if (opt == "--max_columns")
                opts[INDEX_ARG_MAX_COLUMNS] = value;
        }
    }
    return opts;
}
