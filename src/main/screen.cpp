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
 * Definitions of all functions used to print to the screen.
*/

#include <string>

#include <vector>
#include <map>

#include <algorithm>

#include <chrono>

#include <experimental/filesystem>

#include <menu.h>
#include <ncurses.h>

#include "cliex.hpp"
#include "args.hpp"
#include "screen.hpp"

namespace fs = std::experimental::filesystem;

WINDOW *cliex::screen::create_win(int height, int width, int starty, int startx, const std::string &title) noexcept
{
    WINDOW *win;
    win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    keypad(win, 1);

    mvwaddstr(win, 1, 2, title.c_str());
    return win;
}

MENU *cliex::screen::add_file_menu(
    WINDOW *win,
    std::vector<std::string> &choices,
    std::vector<ITEM *> &items,
    const fs::path &current_dir,
    unsigned int max_columns) noexcept

{
    std::string current_dir_s = current_dir.string();
    unsigned longest = 0;

    std::sort(choices.begin(), choices.end(), [](const std::string &a, const std::string &b) {
        return a < b;
    });

    std::transform(choices.begin(), choices.end(), std::back_inserter(items), [&](const std::string &s) -> ITEM * {
        ITEM *item = new_item(s.c_str(), "");
        try
        {
            const fs::path tmp = current_dir / s;
            if (fs::is_directory(fs::symlink_status(tmp)))
                fs::directory_iterator{tmp};
        }
        catch (...)
        {
            item_opts_off(item, O_SELECTABLE);
        }
        return item;
    });
    items.emplace_back(nullptr);

    MENU *menu = new_menu(const_cast<ITEM **>(items.data()));
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, SUB_HEIGHT, SUB_WIDTH, 3, 3));

    for (const auto &c : choices) {
        if (c.length() > longest)
            longest = c.length();
    }

    set_menu_format(menu, LINES - 5, max_columns < SUB_WIDTH / longest ? max_columns : SUB_WIDTH / longest);
    set_menu_mark(menu, "");
    set_menu_grey(menu, COLOR_PAIR(color_pair_inaccessible_dir));

    wmove(win, 1, 1);
    wclrtoeol(win);
    wattron(win, A_BOLD);
    mvwaddstr(win, 1, EXPLORER_WIN_WIDTH - current_dir_s.length() - 2, current_dir_s.c_str());
    wattroff(win, A_BOLD);

    box(win, 0, 0);

    post_menu(menu);
    return menu;
}

void cliex::screen::clear_menu(MENU *menu, std::vector<ITEM *> &items) noexcept
{
    // TODO use the `menu_items` function and remove the `items` parameter
    unpost_menu(menu);
    std::for_each(items.begin(), items.end(), free_item);
    items.clear();
    free_menu(menu);
}