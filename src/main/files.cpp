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
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <experimental/filesystem>
#include <functional>
#include <pwd.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace chrono = std::chrono;
namespace fs = std::experimental::filesystem;

static fs::file_time_type symlink_last_write_time(const fs::path &path)
{
    using clock = fs::file_time_type::clock;

    struct stat sb;
    if (lstat(path.c_str(), &sb) == -1) {
        // TODO proper error handling
        exit(EXIT_FAILURE);
    }

    auto dur = chrono::seconds(sb.st_mtim.tv_sec) +
        chrono::nanoseconds(sb.st_mtim.tv_nsec);
    return clock::time_point(chrono::duration_cast<clock::duration>(dur));
}

fs::path cliex::get_root_path() noexcept
{
    return fs::absolute(fs::current_path()).root_path();
}

fs::path cliex::get_home_dir() noexcept
{
    const std::string HOME = getenv("HOME");
    if(!HOME.empty()) return fs::absolute(HOME);

    const std::string pw_dir = getpwuid(getuid())->pw_dir;
    if(!pw_dir.empty()) return fs::absolute(pw_dir);

    return fs::absolute(fs::current_path());
}

cliex::file_info cliex::get_file_info(
    const fs::path &path,
    const type_config &type_config)
{
    fs::file_status stat = fs::symlink_status(path);
    std::string name = path.filename();
    fs::file_type type = stat.type();
    fs::perms perms = stat.permissions();
    fs::file_time_type last_write_time = symlink_last_write_time(path);

    if (type == fs::file_type::symlink) {
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

    if (type == fs::file_type::directory) {
        bool has_access = true;
        size_t subdirsc = 0;
        size_t filesc = 0;

        try {
            for (const auto &p : fs::directory_iterator(path)) {
                if (fs::is_directory(p))
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

    std::string type_desc;

    if (type != fs::file_type::regular) {
        switch (type) {
        case fs::file_type::block:
            type_desc = "Block Device";
            break;
        case fs::file_type::character:
            type_desc = "Character Device";
            break;
        case fs::file_type::fifo:
            type_desc = "Named IPC Pipe";
            break;
        case fs::file_type::socket:
            type_desc = "Named IPC Socket";
            break;
        case fs::file_type::none:
            type_desc = "None [ERROR STATE]";
            break;
        case fs::file_type::not_found:
            type_desc = "Not Found [ERROR STATE]";
            break;
        case fs::file_type::unknown:
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

    bool executable = (perms & (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec)) != fs::perms::none;

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

std::string cliex::perms_to_string(fs::perms perms) noexcept
{
    std::string str;

    std::function<void(fs::perms, char)> tmp = [&](fs::perms test_perms, char c) {
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
