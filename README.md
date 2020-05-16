# CLIEx
A simple terminal-based file explorer written in C++ using the `ncurses` lib.

## Compile & Run

Make sure you have `ncurses` installed. If not, please do it by typing this command in a terminal (tested on Ubuntu):

`sudo apt-get install libncurses5-dev libncursesw5-dev`

Compile with:

`make`

And run:

`./cliex`

## Usage

### Command line arguments

You can parse arguments at startup using this format: `KEY=VALUE`.

| key           | possible values | description                                         |
| ------------- | --------------- | --------------------------------------------------- |
| `show_hidden` | `true`, `false` | Like in `nautilus`, you can show/hide hidden files. |
|               |                 |                                                     |
|               |                 |                                                     |

You can enter directories with *ENTER*. Quit with *q*.

## Screenshots

![Screenshot](screenshot.png)

## Bug reporting

If you find a bug, please let me know about that. Simply take a look at the issues pages, create an issue and I will try to fix it.