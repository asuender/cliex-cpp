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

using cliex::type_config;
using std::getline;
using std::ifstream;
using std::istream;
using std::istringstream;
using std::map;
using std::ostream;
using std::string;
using std::vector;

type_config type_config::parse(const string &str)
{
    istringstream s(str);
    return read_from(s);
}

type_config type_config::read_from(istream &stream)
{
    map<string, string> types;

    string line, desc;
    vector<string> exts;
    size_t pos_equal;
    while (getline(stream, line))
    {
        line = utils::trim(line);

        if (line.empty() || (line[0] == '/' || line[0] == '#' || line[0] == ';'))
            continue;

        pos_equal = line.find('=');
        if(pos_equal == string::npos) continue;

        exts = utils::split(line.substr(0, pos_equal));
        desc = utils::trim(line.substr(pos_equal + 1));

        for (const auto &e : exts)
            types[e] = desc;
    }

    return type_config(types);
}

type_config type_config::read_from(const string &file_path)
{
    ifstream stream(file_path);
    return read_from(stream);
}

void type_config::merge_with(const type_config &other_config) noexcept
{
    for (const auto &pair : other_config._types)
    {
        if(this->_types.count(pair.first) > 0) continue;
        this->_types[pair.first] = pair.second;
    }
}

type_config type_config::merged_with(const type_config &other_config) const noexcept
{
    type_config ret(*this);
    ret.merge_with(other_config);
    return ret;
}

ostream& operator<<(ostream& stream, const type_config &config)
{
    for (const auto &pair : config.types())
    {
        stream << pair.first << " = " << pair.second << '\n';
    }
    return stream;
}
