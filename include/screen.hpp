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
 * screen.hpp
 *
 * Declarations of all functions used to print to the screen.
 */

#ifndef _SCREEN_HPP
#define _SCREEN_HPP

#include <experimental/filesystem>
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

namespace cliex::screen {
    constexpr short color_pair_inaccessible_dir = 1;

    WINDOW *create_win(
        int height,
        int width,
        int starty,
        int startx,
        const std::string &title) noexcept;
    MENU *add_file_menu(
        WINDOW *win,
        std::vector<std::string> &choices,
        std::vector<ITEM *> &items,
        const std::experimental::filesystem::path &current_dir,
        unsigned int max_columns) noexcept;
    void clear_menu(MENU *menu, std::vector<ITEM *> &items) noexcept;
}

#endif /* _SCREEN_HPP */