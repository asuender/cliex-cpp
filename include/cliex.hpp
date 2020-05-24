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

/**
 * cliex.hpp
 *
 * Declarations of all functions used in the main.cpp file
 */

#ifndef _CLIEX_HPP
#define _CLIEX_HPP

#include <experimental/filesystem>
#include <string>
#include <vector>

namespace cliex {
    constexpr short color_pair_inaccessible_dir = 1;

    void get_dir_content(
        std::vector<std::string> &choices,
        const std::experimental::filesystem::path &current_dir,
        bool show_hidden_files);
}

#endif /* _CLIEX_HPP */
