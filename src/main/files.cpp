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
#include <cstdlib>
#include <experimental/filesystem>
#include <functional>
#include <pwd.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::experimental::filesystem;
using fs::absolute;
using fs::current_path;
using fs::path;
using std::function;
using std::string;

path cliex::get_root_path() noexcept
{
    return fs::absolute(fs::current_path()).root_path();
}

path cliex::get_home_dir() noexcept
{
    const string HOME = getenv("HOME");
    if(!HOME.empty()) return absolute(HOME);

    const string pw_dir = getpwuid(getuid())->pw_dir;
    if(!pw_dir.empty()) return absolute(pw_dir);

    return absolute(current_path());
}

string cliex::perms_to_string(fs::perms perms) noexcept
{
    string str;

    function<void(fs::perms, char)> tmp = [&](fs::perms test_perms, char c) {
        str += (((perms & test_perms) == test_perms) ? c : '-');
    };

    tmp(fs::perms::owner_read, 'r');
    tmp(fs::perms::owner_write, 'w');
    tmp(fs::perms::owner_exec, 'x');

    tmp(fs::perms::group_read, 'r');
    tmp(fs::perms::group_write, 'w');
    tmp(fs::perms::group_exec, 'x');

    tmp(fs::perms::others_read, 'r');
    tmp(fs::perms::others_write, 'w');
    tmp(fs::perms::others_exec, 'x');

    return str;
}
