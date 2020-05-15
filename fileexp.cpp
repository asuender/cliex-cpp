#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <experimental/filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <menu.h>
#include <ncurses.h>

namespace fs = std::experimental::filesystem;

const struct passwd *pw = getpwuid(getuid());
const char* home_dir = pw->pw_dir;

void get_dir_content (const char* s, std::vector<std::string> &v) {
    fs::path path(s);
    fs::directory_iterator beg (path);
    fs::directory_iterator end;
    v.emplace_back(".");
    v.emplace_back("..");
    std::transform(beg, end, std::back_inserter(v), [](const fs::directory_entry &e)
        -> std::string {
        auto p = e.path();
        auto status = fs::status(p);
        std::string s = p.filename().string();
        if (fs::is_directory(status)) { s += "/"; }
        return s;
    });
}

WINDOW* create_win(int height, int width, int starty, int startx) {
    WINDOW *win;
    win = newwin(LINES-1, COLS/2+15, 1, 1);
    box(win, 0, 0);
    keypad(win, 1);
    return win;
}

MENU* create_menu_and_items(WINDOW* win, std::vector<std::string> &choices, std::vector<ITEM*> &items, fs::path current_dir) {
    for (auto &e : choices) {
        items.emplace_back(new_item(e.c_str(), ""));
    }
    items.emplace_back(nullptr);

    MENU *menu = new_menu(const_cast<ITEM**>(items.data()));

    set_menu_win (menu, win);
    set_menu_sub (menu, derwin(win, LINES-5, COLS/2+4, 3, 3));
    set_menu_format(menu, LINES-5, 2);
    set_menu_mark(menu, "");

    mvwaddstr(win, 1, 2, "***** CLI File Explorer *****");
    move(LINES-3, COLS/2+17); clrtoeol();
    mvaddstr(LINES-3, COLS/2+17, ("Current dir: "+current_dir.string()).c_str());
    mvaddstr(LINES-2, COLS/2+17, "Quit by pressing q.");

    post_menu(menu);
    return menu;
}

void delete_and_clear(WINDOW* win, MENU* menu, std::vector<ITEM*> &items) {
    unpost_menu(menu);
    free_menu(menu);
    for (auto &it : items) free_item(it);
    delwin(win);
}

int main(int argc, char const *argv[])
{
    fs::path current_dir(home_dir);
    std::vector<std::string> choices {};
    get_dir_content(current_dir.string().c_str(), choices);
    std::vector<ITEM*> items;
    WINDOW *main;
    MENU *menu;

    int index;
    int c;
    bool fin = false;

    initscr();
    clear();
    noecho();
    curs_set(0);
    cbreak();
    nl();
    keypad(stdscr, 1);

    main = create_win(LINES-1, COLS/+15, 1, 1);
    menu = create_menu_and_items(main, choices, items, current_dir);

    refresh();
    wrefresh(main);

    while ((c=getch()) != 113 && !fin) {
        index = item_index(current_item(menu));
        switch (c) {
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
                if (choices.at(index) == "..") {
                    current_dir = current_dir.parent_path().parent_path();
                } else
                    current_dir = current_dir / choices.at(index);
                auto status = fs::status(current_dir);
                
                if (fs::is_directory(status)) {
                    choices.clear();

                    // delete current menu and create new one
                    items.clear();
                    delete_and_clear(main, menu, items);

                    get_dir_content(current_dir.string().c_str(), choices);
                    main = create_win(LINES-1, COLS/+15, 1, 1);
                    menu = create_menu_and_items(main, choices, items, current_dir);
                    refresh();
                }
        }
        wrefresh(main);
    }

    delete_and_clear(main, menu, items);
    endwin();

    return 0;
}