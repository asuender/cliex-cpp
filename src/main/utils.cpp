#include "utils.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

using std::find_if;
using std::string;
using std::vector;

string trim(const string &source, const string &delims)
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
