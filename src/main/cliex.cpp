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
 * Definitions of all functions used in the main.cpp file.
 */

#include "args.hpp"
#include "cliex.hpp"
#include "files.hpp"
#include <algorithm>
#include <experimental/filesystem>
#include <iterator>
#include <menu.h>
#include <ncurses.h>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;
using std::literals::string_literals::operator""s;

void cliex::get_dir_content(
    std::vector<std::string> &choices,
    const fs::path &current_dir,
    bool show_hidden_files)

{
    std::string s = current_dir.string();
    fs::path path(current_dir);

    fs::directory_iterator beg(path);
    fs::directory_iterator end;

    if (current_dir != get_root_path())
        choices.emplace_back("..");

    std::transform(beg, end, std::back_inserter(choices), [](const fs::directory_entry &e) -> std::string {
        auto p = e.path();
        auto status = fs::status(p);
        std::string s = p.filename().string();
        if (fs::is_directory(status))
        {
            s += "/";
        }
        return s;
    });

    if (!show_hidden_files) {
        choices.erase(std::remove_if(choices.begin(), choices.end(), [](const std::string &s) {
            return s[0] == '.' and s[1] != '.';
        }),
        choices.end());
    }
}
