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

#ifndef _FILES_HPP
#define _FILES_HPP

#include "type_config.hpp"
#include <cstdint>
#include <experimental/filesystem>
#include <string>

namespace cliex {
    struct file_info {
        std::string name;
        std::string type_desc;
        uintmax_t size;
        std::experimental::filesystem::perms perms;
        // TODO user owner and group owner
        std::experimental::filesystem::file_time_type last_write_time;
        // TODO last access time
    };


    std::experimental::filesystem::path get_root_path() noexcept;
    std::experimental::filesystem::path get_home_dir() noexcept;

    file_info get_file_info(
        const std::experimental::filesystem::path& path,
        const type_config& type_config) noexcept;

    std::string perms_to_string(std::experimental::filesystem::perms perms) noexcept;
}

#endif /* _FILES_HPP */
