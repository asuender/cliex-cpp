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

#include "files.hpp"
#include <string>
#include <experimental/filesystem>
#include <functional>

namespace fs = std::experimental::filesystem;
using std::string;
using std::function;

string cliex::perms_to_string(fs::perms perms) noexcept
{
    string str;

    function<char(fs::perms, char)> tmp = [&](fs::perms test_perms, char c) -> char {
        return (((perms & test_perms) == test_perms) ? c : '-');
    };

    str += tmp(fs::perms::owner_read, 'r');
    str += tmp(fs::perms::owner_write, 'w');
    str += tmp(fs::perms::owner_exec, 'x');

    str += tmp(fs::perms::group_read, 'r');
    str += tmp(fs::perms::group_write, 'w');
    str += tmp(fs::perms::group_exec, 'x');

    str += tmp(fs::perms::others_read, 'r');
    str += tmp(fs::perms::others_write, 'w');
    str += tmp(fs::perms::others_exec, 'x');

    return str;
}
