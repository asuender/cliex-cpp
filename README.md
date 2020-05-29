# CLIEx

A simple terminal-based file explorer written in C++ using the `ncurses` lib.

## Compile & Run

If you don't have `ncurses` installed, please do it. The following command works on Ubuntu/Debian:

```sh
sudo apt-get install libncurses5-dev libncursesw5-dev
```

For other distributions, please compile it yourself (get it from [here](https://invisible-island.net/ncurses/)).

Compile/Install with:

```sh
make
sudo make install
```

---

**<u>Note:</u>**  `sudo make install` will copy `default.cfg` from the root of
this repository to `/etc/cliex/`. If you don't want to install the whole program,
copy that file to `~/.config/cliex/` and rename it to `user.cfg`. After that you
can run `cliex` from any folder on your system.

---

And run:

`./cliex`

## Usage

### Command line arguments

Options use the standard GNU long options format.

|    Option     |     Argument     |                                                                               Description                                                                                |
| :------------ | :--------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `show-hidden` | _None_           | Show hidden files                                                                                                                                                        |
| `hide-hidden` | _None_           | Hide hidden files (default)                                                                                                                                              |
| `max-columns` | Positive integer | Set the maximum amount of columns to display in the explorer. If it's bigger than the maximum amount of columns that can be displayed, that maximum will be used instead |

Use the arrow keys to navigate between the items. You can enter directories with *ENTER*. Go back with *DELETE* . Quit with *q*.

## Screenshots

![Screenshot](.github/screenshot.png)

## Resources

* [ncurses](https://invisible-island.net/ncurses/)
* [astyle](http://astyle.sourceforge.net/)

## Bug reporting

If you find a bug, please let me know about that. Simply take a look at the issues pages, create an issue and I will try to fix it.

## License

GPL
