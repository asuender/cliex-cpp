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
 * Definitions of all functions used in the main.cpp file.
 */

#include "args.hpp"
#include "cliex.hpp"
#include "files.hpp"
#include <algorithm>
#include <experimental/filesystem>
#include <iterator>
#include <menu.h>
#include <ncurses.h>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;
using std::literals::string_literals::operator""s;

void cliex::get_dir_content(
    std::vector<std::string> &choices,
    fs::path current_dir,
    bool show_hidden_files)

{
    std::string s = current_dir.string();
    fs::path path(current_dir);

    try {
        fs::directory_iterator beg(path);
        fs::directory_iterator end;

        if (current_dir != get_root_path())
            choices.emplace_back("..");

        std::transform(beg, end, std::back_inserter(choices), [](const fs::directory_entry &e) -> std::string {
            auto p = e.path();
            auto status = fs::status(p);
            std::string s = p.filename().string();
            if (fs::is_directory(status))
            {
                s += "/";
            }
            return s;
        });

        if (!show_hidden_files) {
            choices.erase(std::remove_if(choices.begin(), choices.end(), [](const std::string &s) {
                return s[0] == '.' and s[1] != '.';
            }),
            choices.end());
        }
    }
    catch (...) {
        return;
    }
}

WINDOW* cliex::add_win(int height, int width, int starty, int startx, const char *title = "") noexcept
{
    WINDOW *win;
    win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    keypad(win, 1);

    mvwaddstr(win, 1, 2, title);
    return win;
}

MENU* cliex::add_file_menu(
    WINDOW *win, std::vector<std::string> &choices,
    std::vector<ITEM*> &items,
    fs::path current_dir,
    std::vector<std::string> &opts) noexcept

{
    init_pair(1, COLOR_RED, -1);

    std::string current_dir_s = current_dir.string();
    unsigned longest = 0;
    int max_columns;

    std::sort(choices.begin(), choices.end(), [](const std::string &a, const std::string &b) {
        return a < b;
    });

    std::transform(choices.begin(), choices.end(), std::back_inserter(items), [current_dir](const std::string &s) -> ITEM * {
        ITEM* item = new_item(s.c_str(), "");
        try
        {
            if (!fs::is_directory(fs::status(current_dir/s)))
                fs::file_size(current_dir / s);
            else
                fs::directory_iterator beg(current_dir/s);
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

    for (auto &c : choices) {
        if (c.length() > longest)
            longest = c.length();
    }

    try {
        max_columns = std::stoi(opts[INDEX_ARG_MAX_COLUMNS]);
    }
    catch (...) {
        max_columns = SUB_WIDTH / longest;
    }

    set_menu_format(menu, LINES - 5, max_columns < SUB_WIDTH / longest ? max_columns : SUB_WIDTH / longest);
    set_menu_mark(menu, "");
    set_menu_grey(menu, COLOR_PAIR(1));

    wmove(win, 1, 18);
    wclrtoeol(win);
    wattron(win, A_BOLD);
    mvwaddstr(win, 1, EXPLORER_WIN_WIDTH - current_dir_s.length() - 2, current_dir_s.c_str());
    wattroff(win, A_BOLD);

    box(win, 0, 0);

    post_menu(menu);
    return menu;
}

void cliex::clear_menu(MENU *menu, std::vector<ITEM *> &items) noexcept
{
    // TODO use the `menu_items` function and remove the `items` parameter
    unpost_menu(menu);
    std::for_each(items.begin(), items.end(), free_item);
    items.clear();
    free_menu(menu);
}
