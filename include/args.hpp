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

#ifndef _ARGS_HPP
#define _ARGS_HPP

#include <cstddef>
#include <string>
#include <vector>
#include <cstdint>

namespace cliex::cl {
    struct opts {
        bool show_hidden_files;
        unsigned int max_columns;
    };

    opts parse_argv(int argc, const char *argv[]) noexcept;
}

#endif /* _ARGS_HPP */
