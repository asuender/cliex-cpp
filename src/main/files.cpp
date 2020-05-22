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
#include "type_config.hpp"
#include <algorithm>
#include <cstdlib>
#include <experimental/filesystem>
#include <functional>
#include <pwd.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::experimental::filesystem;
using cliex::file_info;
using cliex::type_config;
using fs::absolute;
using fs::current_path;
using fs::directory_iterator;
using fs::file_size;
using fs::file_status;
using fs::is_block_file;
using fs::is_character_file;
using fs::is_directory;
using fs::is_fifo;
using fs::is_socket;
using fs::is_symlink;
using fs::last_write_time;
using fs::path;
using fs::perms;
using fs::status;
using std::count_if;
using std::function;
using std::string;

path cliex::get_root_path() noexcept
{
    return absolute(current_path()).root_path();
}

path cliex::get_home_dir() noexcept
{
    const string HOME = getenv("HOME");
    if(!HOME.empty()) return absolute(HOME);

    const string pw_dir = getpwuid(getuid())->pw_dir;
    if(!pw_dir.empty()) return absolute(pw_dir);

    return absolute(current_path());
}

file_info cliex::get_file_info(const path& path, const type_config& type_config) noexcept
{
    file_status status = fs::status(path);

    bool is_dir = is_directory(status);
    perms perms = status.permissions();

    string type_desc;

    if (is_dir) type_desc = "Directory";
    else if (is_block_file(status)) type_desc = "Block Device";
    else if (is_character_file(status)) type_desc = "Character Device";
    else if (is_fifo(status)) type_desc = "Named IPC Pipe";
    else if (is_socket(status)) type_desc = "Named IPC Socket";
    else { // regular file
        bool executable = (perms & (perms::owner_exec | perms::group_exec | perms::others_exec)) != perms::none;

        auto types = type_config.types();
        auto it_f = types.find(path.filename());
        auto it_e = types.find(path.extension());

        if (it_f != types.end() || it_e != types.end()) {
            if (it_f != types.end()) {
                type_desc = it_f->second;
            }
            else {
                type_desc = it_e->second;
            }

            if (executable) type_desc += " (Executable)";
        }
        else if (executable) {
            type_desc = "Executable";
        }
        else {
            type_desc = "Unknown";
        }
    }

    if (is_symlink(path)) {
        if(type_desc.empty()) {
            type_desc = "Symlink";
        }
        else {
            type_desc += " (Symlink)";
        }
    }

    uintmax_t size;
    size_t subdirsc;
    size_t filesc;
    if(is_dir) {
        size = 0;
        subdirsc = 0;
        filesc = 0;

        for (const auto &p : directory_iterator(path)) {
            if (is_directory(p))
                ++subdirsc;
            else
                ++filesc;
        }
    }
    else {
        size = file_size(path);
        subdirsc = 0;
        filesc = 0;
    }

    return file_info {
        .name = path.filename(),
        .type = status.type(),
        .type_desc = type_desc,
        .size = size,
        .subdirsc = subdirsc,
        .filesc = filesc,
        .perms = perms,
        .last_write_time = last_write_time(path)
    };
}

string cliex::perms_to_string(perms perms) noexcept
{
    string str;

    function<void(fs::perms, char)> tmp = [&](fs::perms test_perms, char c) {
        str += (((perms & test_perms) == test_perms) ? c : '-');
    };

    tmp(perms::owner_read, 'r');
    tmp(perms::owner_write, 'w');
    tmp(perms::owner_exec, 'x');

    tmp(perms::group_read, 'r');
    tmp(perms::group_write, 'w');
    tmp(perms::group_exec, 'x');

    tmp(perms::others_read, 'r');
    tmp(perms::others_write, 'w');
    tmp(perms::others_exec, 'x');

    return str;
}
