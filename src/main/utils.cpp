/*
 * A simple terminal-based file explorer written in C++ using the ncurses lib.
 * Copyright (C) 2020  Andreas Sünder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 */

#include "utils.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

using std::find_if;
using std::string;
using std::vector;

string cliex::utils::trim(const string &str, const string &trimchars) noexcept
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

vector<string> cliex::utils::split(const string &str) noexcept
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
