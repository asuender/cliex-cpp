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
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <experimental/filesystem>
#include <string>
#include <variant>

namespace cliex {
    struct regular_file_info {
        uintmax_t size;
    };
    struct dir_info {
        /** `true` if current user has read and execute permissions to the dir. */
        bool has_access;
        size_t subdirsc;
        size_t filesc;
    };
    struct symlink_info {
        std::experimental::filesystem::path target;
    };

    struct file_info {
        std::string name;

        std::string type_desc;

        std::experimental::filesystem::file_type type;
        std::experimental::filesystem::perms perms;
        // TODO hard link count
        // TODO user owner and group owner
        std::experimental::filesystem::file_time_type last_write_time;
        // TODO last access time

        /**
         * Will hold a `regular_file_info` if `type` is `file_type::regular`.
         * Will hold a `dir_info`          if `type` is `file_type::directory`.
         * Will hold a `symlink_info`      if `type` is `file_type::symlink`.
         *
         * Will hold a `std::monostate`    if `type` is anything else.
         */
        std::variant<
            // *INDENT-OFF*
            std::monostate,
            regular_file_info,
            dir_info,
            symlink_info
            // TODO add other types as well (character block, fifo, ...)
            // *INDENT-ON*
            > extra_info;
    };


    std::experimental::filesystem::path get_root_path() noexcept;
    std::experimental::filesystem::path get_home_dir() noexcept;

    /**
     * Makes `path` absolutes and resolves "." and ".." path components.
     *
     * Also makes sure that there will not be a trailing empty component.
     */
    std::experimental::filesystem::path resolve(const std::experimental::filesystem::path &path);

    file_info get_file_info(
        const std::experimental::filesystem::path &path,
        const type_config &type_config);

    std::string perms_to_string(std::experimental::filesystem::perms perms) noexcept;

    time_t file_time_type_to_time_t(
        const std::experimental::filesystem::file_time_type &time_point);
}

#endif /* _FILES_HPP */
