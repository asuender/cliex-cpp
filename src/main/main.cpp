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
#include <array>
#include <experimental/filesystem>
#include <fstream>
#include <functional>
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
using std::make_pair;

cliex::type_config setup_type_config() noexcept;
void show_file_info(WINDOW *window, const cliex::file_info &file_info) noexcept;

int main(int argc, const char *argv[])
{
    auto opts = parse_argv(argc, argv);
    bool show_hidden_files = opts[INDEX_ARG_HIDDEN_FILES] == "true";

    cliex::type_config type_config = setup_type_config();
    auto ftypes = type_config.types();

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

    WINDOW *explorer_win = cliex::add_win(EXPLORER_WIN_HEIGHT, EXPLORER_WIN_WIDTH, 1, 1, "***** CLIEx *****");
    MENU *menu = nullptr;

    WINDOW *file_info_win = cliex::add_win(PROPERTY_WIN_HEIGHT, PROPERTY_WIN_WIDTH, 1, EXPLORER_WIN_WIDTH + 2, "File Information");

    mvaddstr(LINES - 2, SUB_WIDTH + 7, ("Quit by pressing q."));

    std::vector<std::string> choices;
    std::vector<ITEM*> items;
    cliex::file_info selected_file_info;
    fs::path current_dir;
    std::string selected;
    std::function<void(const fs::path &newdir)> change_dir = [&](const fs::path &newdir) {
        if (!fs::is_directory(newdir)) return;

        choices.clear();
        cliex::clear_menu(menu, items);

        cliex::get_dir_content(choices, newdir, show_hidden_files);
        std::sort(choices.begin(), choices.end(), [](const std::string &a, const std::string &b) {
            return a < b;
        });

        menu = cliex::add_file_menu(explorer_win, choices, items, newdir, opts);
        selected = item_name(current_item(menu));

        current_dir = newdir;
        chdir(current_dir.c_str());
    };

    change_dir(cliex::get_home_dir());

    for (bool running = true; running; ) {
        // show file info
        selected = item_name(current_item(menu));
        {
            fs::path tmp = current_dir / selected;
            // this is needed because directory items have a slash at the end of
            // their names. that should probably be changed TODO
            if (tmp.filename() == ".") tmp = tmp.parent_path();
            selected_file_info = cliex::get_file_info(tmp, type_config);
        }
        show_file_info(file_info_win, selected_file_info);

        // refresh screen
        refresh();
        wrefresh(explorer_win);
        wrefresh(file_info_win);

        // wait for input and handle
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
            if (selected == "..") {
                change_dir(current_dir.parent_path());
            }
            else if (*(selected.end()-1) == '/') {
                try {
                    fs::directory_iterator subdir_it(current_dir / selected);
                }
                catch (...) {
                    break;
                }

                selected.erase(selected.end()-1);
                change_dir(current_dir / selected);
            }
            break;
        case KEY_BACKSPACE:
            change_dir(current_dir.parent_path());
            break;
        }
    }

    cliex::clear_menu(menu, items);
    delwin(file_info_win);
    delwin(explorer_win);
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

void show_file_info(WINDOW *window, const cliex::file_info &file_info) noexcept
{
    const std::array<std::string, 5> units {"Byte", "KB", "MB", "GB", "TB"};
    constexpr std::array<std::pair<int, int>, 5> window_info_positions {
        make_pair(3, 3),
        make_pair(4, 3),
        make_pair(6, 3),
        make_pair(7, 3),
        make_pair(8, 3)
    };

    // clear the lines were we show the infos
    for (const auto &p : window_info_positions) {
        wmove(window, p.first, p.second);
        wclrtoeol(window);
    }

    // file name
    std::string selected_file_name = file_info.name;
    mvwaddstr(window, 3, 3, selected_file_name.c_str());

    // file type
    std::string selected_file_type_desc = "Type: " + file_info.type_desc;
    mvwaddstr(window, 4, 3, selected_file_type_desc.c_str());

    // file permissions
    std::string selected_file_perms = "Permissions: " + cliex::perms_to_string(file_info.perms);
    mvwaddstr(window, 6, 3, selected_file_perms.c_str());

    // file size
    std::string selected_file_size = "Size: ";
    if (file_info.type != fs::file_type::directory) {
        // TODO use a double if over 1024
        uintmax_t size = file_info.size;
        for (size_t i = 0; ; ++i) {
            if (size < 1024) {
                selected_file_size += std::to_string(size) + ' ' + units[i];
                break;
            }
            size /= 1024;
        }
    }
    else {
        selected_file_size += std::to_string(file_info.subdirsc);
        if (file_info.subdirsc == 1) {
            selected_file_size += " subdirectory";
        }
        else {
            selected_file_size += " subdirectories";
        }

        selected_file_size += ", " + std::to_string(file_info.filesc);
        if (file_info.filesc == 1) {
            selected_file_size += " file";
        }
        else {
            selected_file_size += " files";
        }
    }
    mvwaddstr(window, 7, 3, selected_file_size.c_str());

    // last write time
    fs::file_time_type ftime = file_info.last_write_time;
    time_t cftime = fs::file_time_type::clock::to_time_t(ftime);
    std::string selected_file_last_write_time = "Last mod.: "s + std::asctime(std::localtime(&cftime));
    mvwaddstr(window, 8, 3, selected_file_last_write_time.c_str());

    box(window, 0, 0);
}
