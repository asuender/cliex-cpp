#include "args.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

using std::string;
using std::vector;

vector<string> parse_argv(int argc, const char *argv[])
{
    vector<string> args(argv, argv + argc);
    vector<string> opts(10);
    size_t pos_equal;
    string opt, value;
    for (auto &a : args) {
        pos_equal = a.find("=");
        if (pos_equal != a.npos) {
            opt = cliex::utils::trim(a.substr(0, pos_equal));
            value = cliex::utils::trim(a.substr(pos_equal + 1));
            if (opt == "--show_hidden")
                opts[INDEX_ARG_HIDDEN_FILES] = value;
            else if (opt == "--max_columns")
                opts[INDEX_ARG_MAX_COLUMNS] = value;
        }
    }
    return opts;
}
