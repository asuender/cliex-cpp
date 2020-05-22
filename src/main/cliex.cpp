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

#include <fstream>

#include <string>

#include <vector>
#include <map>

#include <algorithm>

#include <chrono>

#include <experimental/filesystem>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <menu.h>
#include <ncurses.h>

#include "cliex.hpp"

namespace fs = std::experimental::filesystem;
using std::literals::string_literals::operator""s;

std::map<std::string, std::string> cliex::get_all_types()
{
    if (!fs::exists(USER_TYPES_PATH)) {
        fs::create_directory(USER_TYPES_PATH.parent_path());
        fs::copy(DEFAULT_TYPES_PATH, USER_TYPES_PATH);
    }

    auto user_types = load_config(USER_TYPES_PATH);

    if (fs::exists(DEFAULT_TYPES_PATH)) {
        auto default_types = load_config(DEFAULT_TYPES_PATH);
        if (user_types.size() != default_types.size() || !std::equal(user_types.begin(), user_types.end(), default_types.begin())) {
            default_types.insert(user_types.begin(), user_types.end());
            std::swap(user_types, default_types);
            std::fstream overwrite{USER_TYPES_PATH, std::ios::out | std::ios::trunc};
            overwrite << "# Configuration file for cliex.\n# It is used by the file explorer to detect file types correctly.\n\n";
            for (const auto &t : user_types)
                overwrite << t.first << " = " << t.second << "\n";
            overwrite.clear();
            overwrite.close();
        }
    }
    return user_types;
}

std::string cliex::get_type(fs::path path, fs::perms p, std::map<std::string, std::string> &ftypes)
{
    auto filename = path.filename().string();
    auto extension = path.extension().string();
    std::string type;

    if (fs::is_block_file(path)) type = "block device";
    else if (fs::is_character_file(path)) type = "character device";
    else if (fs::is_fifo(path)) type = "named IPC pipe";
    else if (fs::is_socket(path)) type = "named IPC socket";
    else if (fs::is_symlink(path)) type = "symlink";
    else {
        auto it_f = ftypes.find(filename);
        auto it_e = ftypes.find(extension);

        type = (it_f != ftypes.end()) ? it_f->second : (it_e != ftypes.end()) ? it_e->second : ((p & fs::perms::owner_exec) != fs::perms::none || (p & fs::perms::group_exec) != fs::perms::none || (p & fs::perms::others_exec) != fs::perms::none) ? "Executable" : "Unknown";
    }

    return type;
}

std::string cliex::get_perms(fs::path path)
{
    auto p = fs::status(path).permissions();
    std::string perms_s = "";
    perms_s += ((p & fs::perms::owner_read) != fs::perms::none ? "r"s : "-"s) + ((p & fs::perms::owner_write) != fs::perms::none ? "w"s : "-"s) + ((p & fs::perms::owner_exec) != fs::perms::none ? "x"s : "-"s) + ((p & fs::perms::group_read) != fs::perms::none ? "r"s : "-"s) + ((p & fs::perms::group_write) != fs::perms::none ? "w"s : "-"s) + ((p & fs::perms::group_exec) != fs::perms::none ? "x"s : "-"s) + ((p & fs::perms::others_read) != fs::perms::none ? "r"s : "-"s) + ((p & fs::perms::others_write) != fs::perms::none ? "w"s : "-"s) + ((p & fs::perms::others_exec) != fs::perms::none ? "x"s : "-"s);
    return perms_s;
}

std::map<std::string, std::string> cliex::load_config(std::string file)
{
    std::map<std::string, std::string> content;
    std::fstream cf{file, std::ios::in};

    std::string line, value;
    std::vector<std::string> exts;
    int pos_equal;
    while (std::getline(cf, line)) {
        if (!line.length())
            continue;

        if (line[0] == '/' || line[0] == '#' || line[0] == ';') continue;

        pos_equal = line.find('=');
        value = trim(line.substr(pos_equal + 1));
        exts = split(line.substr(0, pos_equal));

        for (const auto &e : exts)
            content[e] = value;
    }
    cf.close();
    return content;
}

void cliex::get_dir_content(
    std::vector<std::string> &choices,
    fs::path current_dir,
    std::vector<std::string> &opts)

{
    std::string s = current_dir.string();
    fs::path path(current_dir);

    try {
        fs::directory_iterator beg(path);
        fs::directory_iterator end;

        if (current_dir != ROOT_DIR)
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

        if (opts[INDEX_ARG_HIDDEN_FILES] == "false") {
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

WINDOW* cliex::add_win(int height, int width, int starty, int startx, const char *title = "")
{
    WINDOW *win;
    win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    keypad(win, 1);

    mvwaddstr(win, 1, 2, title);
    return win;
}

MENU* cliex::add_file_menu(
    WINDOW *main, std::vector<std::string> &choices,
    std::vector<ITEM *> &items,
    fs::path current_dir,
    std::vector<std::string> &opts)

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
    set_menu_win(menu, main);
    set_menu_sub(menu, derwin(main, SUB_HEIGHT, SUB_WIDTH, 3, 3));

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

    wmove(main, 1, 18);
    wclrtoeol(main);
    wattron(main, A_BOLD);
    mvwaddstr(main, 1, MAIN_WIDTH - current_dir_s.length() - 2, current_dir_s.c_str());
    wattroff(main, A_BOLD);

    post_menu(menu);
    return menu;
}

void cliex::clear_menu(MENU *menu, std::vector<ITEM *> &items)
{
    unpost_menu(menu);
    free_menu(menu);
    for (auto &it : items)
        free_item(it);
}

void cliex::show_file_info(WINDOW *property_win,
                           std::string &selected,
                           fs::path full_path,
                           std::map<std::string, std::string> &ftypes)
{
    using std::make_pair;
    using namespace std::chrono_literals;

    auto status = fs::status(full_path);
    auto is_dir = fs::is_directory(status);
    bool has_perms = true;
    size_t size, c_dirs=0, c_files=0;

    std::vector<std::string> units {
        "Byte", "KB", "MB", "GB", "TB"};

    std::vector<std::pair<int, int>> line_pos {
        make_pair(3, 3),
        make_pair(4, 3),
        make_pair(6, 3),
        make_pair(7, 3),
        make_pair(8, 3)};

    for (auto &p : line_pos) {
        wmove(property_win, p.first, p.second);
        wclrtoeol(property_win);
    }

    mvwaddstr(property_win, 3, 3, selected.c_str());
    mvwaddstr(property_win, 4, 3, ("Type: "s + (is_dir ? "directory" : get_type(full_path, status.permissions(), ftypes))).c_str());

    mvwaddstr(property_win, 6, 3, ("Permissions: "s + get_perms(full_path)).c_str());

    if (!is_dir) {
        try {
            size = fs::file_size(full_path);
        }
        catch (...) {
            has_perms = false;
        }

        for (int i = 0; has_perms; i++) {
            if (size < 1024) {
                mvwaddstr(property_win, 7, 3, ("Size: " + std::to_string(size) + " " + units[i]).c_str());
                break;
            }
            size /= 1024;
        }
    }
    else {
        try {
            using fp = bool (*)(const fs::path&);

            c_files = std::count_if(fs::directory_iterator(full_path), fs::directory_iterator{}, (fp)fs::is_regular_file);
            c_dirs = std::count_if(fs::directory_iterator(full_path), fs::directory_iterator{}, (fp)fs::is_directory);
            mvwaddstr(property_win, 7, 3, ("Size: " + std::to_string(c_dirs) + " subfolders, " + std::to_string(c_files) + " files").c_str());

        }
        catch (...) {
            has_perms = false;
        }
    }

    if (!has_perms)
        mvwaddstr(property_win, 7, 3, "Size: Unknown");
    try {
        auto ftime = fs::last_write_time(full_path);
        std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
        mvwaddstr(property_win, 8, 3, ("Last mod.: "s + std::asctime(std::localtime(&cftime))).c_str());
    }
    catch(...) {
        mvwaddstr(property_win, 8, 3, "Last mod.: unknown");
    }

}

void cliex::update(std::vector<WINDOW*> windows)
{
    refresh();
    std::for_each(windows.begin(), windows.end(), [](WINDOW* win) {
        box(win, 0, 0);
        wrefresh(win);
    });
}
