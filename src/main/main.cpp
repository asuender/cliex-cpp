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
 * cliex.cpp
 *
 * only contains the main function.
 */

#include "args.hpp"
#include "cliex.hpp"
#include "files.hpp"
#include "type_config.hpp"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <menu.h>
#include <ncurses.h>
#include <pwd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

namespace fs = std::experimental::filesystem;
using std::literals::string_literals::operator""s;

cliex::type_config setup_type_config() noexcept;

int main(int argc, const char *argv[])
{
    auto opts = parse_argv(argc, argv);
    bool show_hidden_files = opts[INDEX_ARG_HIDDEN_FILES] == "true";

    cliex::type_config type_config = setup_type_config();
    auto ftypes = type_config.types();

    std::vector<std::string> choices{};
    std::string selected;
    fs::path current_dir = cliex::get_home_dir();
    cliex::get_dir_content(choices, current_dir, show_hidden_files);

    std::vector<ITEM*> items;
    WINDOW *explorer_win, *file_info_win;
    MENU *menu;

    setlocale(LC_ALL, "");
    initscr();
    clear();
    noecho();
    curs_set(0);
    cbreak();
    nl();
    use_default_colors();
    keypad(stdscr, 1);

    start_color();

    explorer_win = cliex::add_win(MAIN_HEIGHT, MAIN_WIDTH, 1, 1, "***** CLIEx *****");
    menu = cliex::add_file_menu(explorer_win, choices, items, current_dir, opts);

    file_info_win = cliex::add_win(PROPERTY_WIN_HEIGHT, PROPERTY_WIN_WIDTH, 1, MAIN_WIDTH + 2, "File Information");

    mvaddstr(LINES - 2, SUB_WIDTH + 7, ("Quit by pressing q."));

    cliex::update({explorer_win, file_info_win});

    for (bool running = true; running; ) {
        switch (getch()) {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case KEY_RIGHT:
            menu_driver(menu, REQ_RIGHT_ITEM);
            break;
        case KEY_LEFT:
            menu_driver(menu, REQ_LEFT_ITEM);
            break;
        case KEY_NPAGE:
            menu_driver(menu, REQ_SCR_DPAGE);
            break;
        case KEY_PPAGE:
            menu_driver(menu, REQ_SCR_UPAGE);
            break;
        case 'q':
            running = false;
            break;
        case '\n':
            selected = item_name(current_item(menu));
            if (selected == "..") {
                current_dir = current_dir.parent_path();
                goto change_dir;
            }
            else if (*(selected.end()-1) == '/') {
                try {
                    fs::directory_iterator subdir_it(current_dir / selected);
                }
                catch (...) {
                    break;
                }

                selected.erase(selected.end()-1);
                current_dir = current_dir / selected;
                goto change_dir;
            }
            break;

        case KEY_BACKSPACE:
            selected = item_name(current_item(menu));
            if (current_dir != cliex::get_root_path()) {
                current_dir = current_dir.parent_path();
                goto change_dir;
            }
            break;

change_dir:
            if (fs::is_directory(fs::status(current_dir))) {
                choices.clear();
                items.clear();
                cliex::clear_menu(menu, items);

                cliex::get_dir_content(choices, current_dir, show_hidden_files);
                menu = cliex::add_file_menu(explorer_win, choices, items, current_dir, opts);
                selected = item_name(current_item(menu));
            }
        }

        selected = item_name(current_item(menu));
        cliex::show_file_info(file_info_win, selected, current_dir / selected, ftypes);
        cliex::update({explorer_win, file_info_win});
    }

    cliex::clear_menu(menu, items);
    delwin(explorer_win);
    delwin(file_info_win);
    endwin();

    return 0;
}

cliex::type_config setup_type_config() noexcept
{
    const fs::path default_type_config_path = cliex::get_root_path() / "etc"     / "cliex" / "default.cfg";
    const fs::path user_type_config_path    = cliex::get_home_dir()  / ".config" / "cliex" / "user.cfg";

    const bool default_type_config_available = fs::exists(default_type_config_path) && fs::is_regular_file(default_type_config_path);
    const bool user_type_config_available    = fs::exists(user_type_config_path)    && fs::is_regular_file(user_type_config_path);

    cliex::type_config default_type_config;
    if (default_type_config_available) {
        default_type_config = cliex::type_config::read_from(default_type_config_path);
    }
    cliex::type_config user_type_config;
    if (user_type_config_available) {
        user_type_config = cliex::type_config::read_from(user_type_config_path);
    }

    if (default_type_config != user_type_config) {
        user_type_config.merge_with(default_type_config);

        // only overwrite the user type config when it exists
        if (user_type_config_available) {
            std::ofstream user_type_config_stream(user_type_config_path);
            user_type_config_stream << "# Configuration file for cliex.\n"
                                    "# It is used by the file explorer to detect file types correctly.\n\n";
            user_type_config_stream << user_type_config;
        }
    }

    return user_type_config;
}
