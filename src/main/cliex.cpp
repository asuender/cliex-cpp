#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <experimental/filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <menu.h>
#include <ncurses.h>

#define ROOT_DIR "/"
#define MAIN_HEIGHT (LINES-1)
#define MAIN_WIDTH (COLS*0.65)
#define SUB_HEIGHT (LINES-5)
#define SUB_WIDTH (COLS*0.65-5)
#define INDEX_OPT_HIDDEN_FILES 0

namespace fs = std::experimental::filesystem;

const struct passwd *pw = getpwuid(getuid());
const char* home_dir = pw->pw_dir;

std::string trim(std::string const &source, char const *delims = " \t\r\n") {
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

std::vector<std::string> parse_argv(int argc, char const *argv[]) {
    std::vector<std::string> args(argv, argv+argc);
    std::vector<std::string> opts (10);
    size_t pos_equal;
    std::string opt, value;
    for (auto &a : args) {
        pos_equal = a.find("=");
        if (pos_equal != a.npos)  {
            opt = trim(a.substr(0, pos_equal));
            value = trim(a.substr(pos_equal+1));
            if (opt == "show_hidden")
                opts[INDEX_OPT_HIDDEN_FILES] = value;
        }
    }
    return opts;
}

void get_dir_content (
    const char* s, std::vector<std::string> &v,
    fs::path current_dir,
    std::vector<std::string> &opts)

{
    fs::path path(s);
    fs::directory_iterator beg (path);
    fs::directory_iterator end;

    if (current_dir != ROOT_DIR)
        v.emplace_back("..");
    std::transform(beg, end, std::back_inserter(v), [](const fs::directory_entry &e)
        -> std::string {
        auto p = e.path();
        auto status = fs::status(p);
        std::string s = p.filename().string();
        if (fs::is_directory(status)) { s += "/"; }
        return s;
    });

    if (opts[INDEX_OPT_HIDDEN_FILES] == "false") {
        v.erase(std::remove_if(v.begin(), v.end(), [](const std::string &s) {
            return s[0] == '.' and s[1] != '.';
        }), v.end());
    }
}

WINDOW* create_win(int height, int width, int starty, int startx) {
    WINDOW *win;
    win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    keypad(win, 1);
    return win;
}

MENU* create_menu_and_items(
    WINDOW* win, std::vector<std::string> &choices,
    std::vector<ITEM*> &items, 
    fs::path current_dir)

{
    std::string current_dir_s = current_dir.string();

    for (auto &e : choices) {
        items.emplace_back(new_item(e.c_str(), ""));
    }
    items.emplace_back(nullptr);

    MENU *menu = new_menu(const_cast<ITEM**>(items.data()));

    set_menu_win (menu, win);
    set_menu_sub (menu, derwin(win, SUB_HEIGHT, SUB_WIDTH, 3, 3));

    unsigned longest = 0;
    for (auto &c : choices) {
        if (c.length() > longest) longest = c.length(); 
    }
    set_menu_format(menu, LINES-5, SUB_WIDTH/longest);
    
    set_menu_mark(menu, "");

    mvwaddstr(win, 1, 2, "***** CLIEx *****");
    move(LINES-3, COLS/2+17); clrtoeol();
    wattron(win, A_BOLD);
    mvwaddstr(win, 1, MAIN_WIDTH-current_dir_s.length()-2, current_dir_s.c_str());
    wattroff(win, A_BOLD);
    mvaddstr(LINES-2, SUB_WIDTH+7, ("Quit by pressing q."));

    post_menu(menu);
    return menu;
}

void delete_and_clear_menu(WINDOW* win, MENU* menu, std::vector<ITEM*> &items) {
    unpost_menu(menu);
    free_menu(menu);
    for (auto &it : items) free_item(it);
    delwin(win);
}

int main(int argc, char const *argv[])
{
    auto opts = parse_argv(argc, argv);

    fs::path current_dir(home_dir);
    std::vector<std::string> choices {};
    get_dir_content(current_dir.string().c_str(), choices, current_dir, opts);

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

    main = create_win(MAIN_HEIGHT, MAIN_WIDTH, 1, 1);
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
                    // delete current menu and create new one

                    choices.clear();
                    items.clear();
                    delete_and_clear_menu(main, menu, items);

                    get_dir_content(current_dir.string().c_str(), choices, current_dir, opts);
                    main = create_win(MAIN_HEIGHT, MAIN_WIDTH, 1, 1);
                    menu = create_menu_and_items(main, choices, items, current_dir);
                    refresh();
                }
        }
        wrefresh(main);
    }

    delete_and_clear_menu(main, menu, items);
    endwin();

    return 0;
}