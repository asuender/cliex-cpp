#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <vector>

using std::find_if;
using std::string;
using std::vector;

string cliex::utils::trim(const string &str, const string &trimchars)
{
    string result(str);
    size_t index = result.find_last_not_of(trimchars);
    if (index != string::npos)
        result.erase(++index);

    index = result.find_first_not_of(trimchars);
    if (index != string::npos)
        result.erase(0, index);
    else
        result.erase();
    return result;
}

vector<string> cliex::utils::split(const string &str)
{
    vector<string> result;
    auto it = str.begin();
    while (it != str.end()) {
        it = find_if(it, str.end(), [](char c) {
            return !isspace(c);
        });
        auto jt = find_if(it, str.end(), [](char c) {
            return isspace(c);
        });
        if (it != str.end())
            result.push_back(string(it, jt));
        it = jt;
    }
    return result;
}
