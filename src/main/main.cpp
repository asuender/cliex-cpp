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

#include <iostream>
#include <fstream>

#include <string>

#include <vector>
#include <map>

#include <iterator>
#include <algorithm>

#include <experimental/filesystem>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <menu.h>
#include <ncurses.h>

#include "cliex.hpp"

namespace fs = std::experimental::filesystem;
using std::literals::string_literals::operator""s;

const struct passwd *pw = getpwuid(getuid());
const char *home_dir = pw->pw_dir;

std::string trim(std::string const &source, char const *delims)
{
    std::string result(source);
    std::string::size_type index = result.find_last_not_of(delims);
    if (index != std::string::npos)
        result.erase(++index);

    index = result.find_first_not_of(delims);
    if (index != std::string::npos)
        result.erase(0, index);
    else
        result.erase();
    return result;
}

std::vector<std::string> split(const std::string &s)
{
    std::vector<std::string> result;
    auto it = s.begin();
    while (it != s.end())
        {
            it = std::find_if(it, s.end(), [](char c)
            {
                return c != ' ';
            });
            auto jt = std::find_if(it, s.end(), [](char c)
            {
                return c == ' ';
            });
            if (it != s.end())
                result.push_back(std::string(it, jt));
            it = jt;
        }
    return result;
}

std::vector<std::string> parse_argv(int argc, char const *argv[])
{
    std::vector<std::string> args(argv, argv + argc);
    std::vector<std::string> opts(10);
    size_t pos_equal;
    std::string opt, value;
    for (auto &a : args)
        {
            pos_equal = a.find("=");
            if (pos_equal != a.npos)
                {
                    opt = trim(a.substr(0, pos_equal));
                    value = trim(a.substr(pos_equal + 1));
                    if (opt == "--show_hidden")
                        opts[INDEX_ARG_HIDDEN_FILES] = value;
                    else if (opt == "--max_columns")
                        opts[INDEX_ARG_MAX_COLUMNS] = value;
                }
        }
    return opts;
}

int main(int argc, char const *argv[])
{
    auto opts = parse_argv(argc, argv);
    auto ftypes = cliex::get_all_types();

    std::vector<std::string> choices{};
    std::string selected;
    fs::path current_dir(home_dir);
    cliex::get_dir_content(choices, current_dir, opts);

    std::vector<ITEM *> items;
    WINDOW *main, *property_win;
    MENU *menu;

    int c;
    bool fin = false;

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

    main = cliex::add_win(MAIN_HEIGHT, MAIN_WIDTH, 1, 1, "***** CLIEx *****");
    menu = cliex::add_file_menu(main, choices, items, current_dir, opts);

    property_win = cliex::add_win(PROPERTY_WIN_HEIGHT, PROPERTY_WIN_WIDTH, 1, MAIN_WIDTH + 2, "File Information");

    mvaddstr(LINES - 2, SUB_WIDTH + 7, ("Quit by pressing q."));

    cliex::update({main, property_win});

    while ((c = getch()) != 113 && !fin)
        {
            auto current_dir_status = fs::status(current_dir);

            switch (c)
                {
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
                case 0xA:
                    selected = item_name(current_item(menu));
                    if (selected == "..")
                        {
                            current_dir = current_dir.parent_path();
                            goto change_dir;
                        }
                    else if (*(selected.end()-1) == '/')
                        {
                            try
                                {
                                    fs::directory_iterator subdir_it(current_dir / selected);
                                }
                            catch (...)
                                {
                                    break;
                                }

                            selected.erase(selected.end()-1);
                            current_dir = current_dir / selected;
                            goto change_dir;
                        }
                    break;

                case KEY_BACKSPACE:
                    selected = item_name(current_item(menu));
                    if (current_dir != ROOT_DIR)
                        {
                            current_dir = current_dir.parent_path();
                            goto change_dir;
                        }
                    break;

change_dir:
                    if (fs::is_directory(fs::status(current_dir)))
                        {
                            choices.clear();
                            items.clear();
                            cliex::clear_menu(menu, items);

                            cliex::get_dir_content(choices, current_dir, opts);
                            menu = cliex::add_file_menu(main, choices, items, current_dir, opts);
                            selected = item_name(current_item(menu));
                        }
                }

            selected = item_name(current_item(menu));
            cliex::show_file_info(property_win, selected, current_dir / selected, ftypes);
            cliex::update({main, property_win});
        }

    cliex::clear_menu(menu, items);
    delwin(main);
    delwin(property_win);
    endwin();

    return 0;
}
