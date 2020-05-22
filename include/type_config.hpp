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

#ifndef _TYPE_CONFIG_HPP
#define _TYPE_CONFIG_HPP

#include <istream>
#include <map>
#include <ostream>
#include <string>

namespace cliex {
    struct type_config {
    private:
        std::map<std::string, std::string> _types;

    public:
        explicit inline type_config(const std::map<std::string, std::string> &types = {})
            : _types(types)
        {}
        static type_config parse(const std::string &str);
        static type_config read_from(std::istream &stream);
        static type_config read_from(const std::string &file_path);

        inline const std::map<std::string, std::string> &types() const noexcept
        {
            return _types;
        }

        inline bool operator==(const type_config& rhs) const noexcept
        {
            return this->_types == rhs._types;
        }
        inline bool operator!=(const type_config& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        void merge_with(const type_config &other_config) noexcept;
        type_config merged_with(const type_config &other_config) const noexcept;
    };
}

std::ostream &operator<<(std::ostream &stream, const cliex::type_config &config);

#endif /* _TYPE_CONFIG_HPP */
