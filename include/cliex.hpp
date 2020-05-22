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

#include <string>

#include <vector>
#include <map>

#include <experimental/filesystem>

#include <menu.h>
#include <ncurses.h>

namespace fs = std::experimental::filesystem;

#define ROOT_DIR "/"
#define DEFAULT_TYPES_PATH (fs::path("/etc/cliex/default.cfg"))
#define USER_TYPES_PATH (fs::path(home_dir) / ".config" / "cliex" / "user.cfg")

#define MAIN_HEIGHT (LINES - 1)
#define MAIN_WIDTH (COLS * 0.65)
#define SUB_HEIGHT (LINES - 5)
#define SUB_WIDTH (COLS * 0.65 - 5)
#define PROPERTY_WIN_HEIGHT (LINES - 5)
#define PROPERTY_WIN_WIDTH (COLS * 0.35 - 1)

#define INDEX_ARG_HIDDEN_FILES 0
#define INDEX_ARG_MAX_COLUMNS 1

extern const char *home_dir;

extern std::string trim(std::string const&, char const* = " \t\r\n");
extern std::vector<std::string> split(const std::string&);

namespace cliex {
    std::map<std::string, std::string> get_all_types();
    std::string get_type(fs::path, fs::perms, std::map<std::string, std::string>&);
    std::string get_perms(fs::path);
    std::map<std::string, std::string> load_config(std::string);

    void get_dir_content(std::vector<std::string>&, fs::path, std::vector<std::string>&);

    WINDOW *add_win(int, int, int, int, const char *);
    MENU *add_file_menu(WINDOW*, std::vector<std::string>&, std::vector<ITEM *>&, fs::path, std::vector<std::string>&);
    void clear_menu(MENU*, std::vector<ITEM *>&);
    void show_file_info(WINDOW*, std::string&, fs::path, std::map<std::string, std::string>&);

    void update(std::vector<WINDOW*>);
}