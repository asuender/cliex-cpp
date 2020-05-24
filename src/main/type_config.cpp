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

#include "type_config.hpp"
#include "utils.hpp"
#include <fstream>
#include <istream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

cliex::type_config cliex::type_config::parse(const std::string &str) noexcept
{
    std::istringstream s(str);
    return read_from(s);
}

cliex::type_config cliex::type_config::read_from(std::istream &stream) noexcept
{
    std::map<std::string, std::string> types;

    std::string line, desc;
    std::vector<std::string> exts;
    size_t pos_equal;
    while (std::getline(stream, line)) {
        size_t comment_pos = line.find_first_of("#;/");
        if (comment_pos != std::string::npos) line.erase(comment_pos);

        line = utils::trim(line);

        if (line.empty()) continue;

        pos_equal = line.find('=');
        if(pos_equal == std::string::npos) continue;

        exts = utils::split(line.substr(0, pos_equal));
        desc = utils::trim(line.substr(pos_equal + 1));

        for (const auto &e : exts)
            types[e] = desc;
    }

    return type_config(types);
}

cliex::type_config cliex::type_config::read_from(const std::string &file_path) noexcept
{
    std::ifstream stream(file_path);
    return read_from(stream);
}

void cliex::type_config::merge_with(const cliex::type_config &other_config) noexcept
{
    for (const auto &pair : other_config._types) {
        if(this->_types.count(pair.first) > 0) continue;
        this->_types[pair.first] = pair.second;
    }
}

cliex::type_config cliex::type_config::merged_with(
    const type_config &other_config) const noexcept
{
    type_config ret(*this);
    ret.merge_with(other_config);
    return ret;
}

std::ostream &operator<<(std::ostream &stream, const cliex::type_config &config)
{
    for (const auto &pair : config.types()) {
        stream << pair.first << " = " << pair.second << '\n';
    }
    return stream;
}
