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

/**
 * cliex.hpp
 *
 * Declarations of all functions used in the main.cpp file
 */

#ifndef _CLIEX_HPP
#define _CLIEX_HPP

#include <experimental/filesystem>
#include <map>
#include <menu.h>
#include <ncurses.h>
#include <string>
#include <vector>

#define EXPLORER_WIN_HEIGHT (LINES - 1)
#define EXPLORER_WIN_WIDTH (COLS * 0.65)
#define SUB_HEIGHT (LINES - 5)
#define SUB_WIDTH (COLS * 0.65 - 5)
#define PROPERTY_WIN_HEIGHT (LINES - 5)
#define PROPERTY_WIN_WIDTH (COLS * 0.35 - 1)

namespace cliex {
    std::string get_type(std::experimental::filesystem::path, std::experimental::filesystem::perms, std::map<std::string, std::string>&);

    void get_dir_content(std::vector<std::string>&, std::experimental::filesystem::path, bool);

    WINDOW *add_win(int, int, int, int, const char*);
    MENU *add_file_menu(WINDOW*, std::vector<std::string>&, std::vector<ITEM*>&, std::experimental::filesystem::path, std::vector<std::string>&);
    void clear_menu(MENU*, std::vector<ITEM*>&);
    void show_file_info(WINDOW*, std::string&, std::experimental::filesystem::path, std::map<std::string, std::string>&);
}

#endif /* _CLIEX_HPP */
