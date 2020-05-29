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
#include <algorithm>
#include <string>

cliex::cl::opts cliex::cl::parse_argv(int argc, const char *argv[]) noexcept
{
    bool show_hidden_files = false;
    unsigned int max_columns = -1;

    std::for_each(argv, argv + argc, [&](const std::string &arg) {
        if (arg.substr(0, 2) != "--") return;

        size_t pos_equal = arg.find('=');
        std::string opt;
        std::string opt_arg;

        // check if option has argument
        if (pos_equal == std::string::npos) {
            opt = arg.substr(2);
        }
        else {
            opt = arg.substr(2, pos_equal - 2);
            opt_arg = arg.substr(pos_equal + 1);
        }

        // handle option
        if (opt == "show-hidden") {
            show_hidden_files = true;
        }
        else if (opt == "hide-hidden") {
            show_hidden_files = false;
        }
        else if (opt == "max-columns") {
            try {
                max_columns = std::stoi(opt_arg);
            }
            catch (...) {}
        }
    });

    return opts {
        .show_hidden_files = show_hidden_files,
        .max_columns = max_columns
    };
}
