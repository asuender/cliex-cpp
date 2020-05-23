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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::experimental::filesystem;
using cliex::dir_info;
using cliex::file_info;
using cliex::symlink_info;
using cliex::type_config;
using fs::absolute;
using fs::current_path;
using fs::directory_iterator;
using fs::file_size;
using fs::file_status;
using fs::file_time_type;
using fs::file_type;
using fs::is_block_file;
using fs::is_character_file;
using fs::is_directory;
using fs::is_fifo;
using fs::is_regular_file;
using fs::is_socket;
using fs::is_symlink;
using fs::last_write_time;
using fs::path;
using fs::perms;
using fs::read_symlink;
using fs::status;
using fs::symlink_status;
using std::count_if;
using std::function;
using std::string;
using std::variant;

static file_time_type symink_last_write_time(const path &path)
{
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;
    using std::chrono::seconds;
    using clock = file_time_type::clock;

    struct stat sb;
    if (lstat(path.c_str(), &sb) == -1) {
        // TODO proper error handling
        exit(EXIT_FAILURE);
    }

    auto dur = seconds(sb.st_mtim.tv_sec) + nanoseconds(sb.st_mtim.tv_nsec);
    return clock::time_point(duration_cast<clock::duration>(dur));
}

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

file_info cliex::get_file_info(
    const path& path,
    const type_config& type_config)
{
    file_status stat = symlink_status(path);
    string name = path.filename();
    file_type type = stat.type();
    perms perms = stat.permissions();
    file_time_type last_write_time = symink_last_write_time(path);

    if (type == file_type::symlink) {
        return file_info {
            .name = name,
            .type_desc = "Symlink",
            .type = type,
            .perms = perms,
            .last_write_time = last_write_time,
            .extra_info = symlink_info {
                .target = read_symlink(path)
            }
        };
    }

    if (type == file_type::directory) {
        bool has_access = true;
        size_t subdirsc = 0;
        size_t filesc = 0;

        try {
            for (const auto &p : directory_iterator(path)) {
                if (is_directory(p))
                    ++subdirsc;
                else
                    ++filesc;
            }
        }
        catch(...) {
            has_access = false;
        }

        return file_info {
            .name = name,
            .type_desc = "Directory",
            .type = type,
            .perms = perms,
            .last_write_time = last_write_time,
            .extra_info = dir_info {
                .has_access = has_access,
                .subdirsc = subdirsc,
                .filesc = filesc
            }
        };
    }

    string type_desc;

    if (type != file_type::regular) {
        switch (type) {
        case file_type::block:
            type_desc = "Block Device";
            break;
        case file_type::character:
            type_desc = "Character Device";
            break;
        case file_type::fifo:
            type_desc = "Named IPC Pipe";
            break;
        case file_type::socket:
            type_desc = "Named IPC Socket";
            break;
        case file_type::none:
            type_desc = "None [ERROR STATE]";
            break;
        case file_type::not_found:
            type_desc = "Not Found [ERROR STATE]";
            break;
        case file_type::unknown:
            type_desc = "Unknown [ERROR STATE]";
            break;
        default:
            type_desc = "[ERROR STATE]";
            break;
        }

        return file_info {
            .name = name,
            .type_desc = type_desc,
            .type = type,
            .perms = perms,
            .last_write_time = last_write_time,
            .extra_info = {}
        };
    }

    uintmax_t size = file_size(path);

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
        type_desc = "Unknown Regular File";
    }

    return file_info {
        .name = name,
        .type_desc = type_desc,
        .type = type,
        .perms = perms,
        .last_write_time = last_write_time,
        .extra_info = regular_file_info {
            .size = size
        }
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
