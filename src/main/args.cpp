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

#include "args.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

using std::string;
using std::vector;

vector<string> parse_argv(int argc, const char *argv[]) noexcept
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
