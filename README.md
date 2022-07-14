# Cute Chess

![GitHub CI](https://github.com/cutechess/cutechess/workflows/build%20cutechess/badge.svg)

Cute Chess is a graphical user interface, command-line interface and a library
for playing chess. Cute Chess is written in C++ using the [Qt
framework](https://www.qt.io/).

## Installation

### Binaries

See the [Releases](https://github.com/cutechess/cutechess/releases) page.

### Building from source

Cute Chess requires Qt 5.15 or greater, a compiler with C++11 support and `cmake`.
Cute Chess depends on the following Qt 5 modules:

* qt5-widgets
* qt5-svg
* qt5-concurrent
* qt5-printsupport
* qt5-testlib (optional: unit tests)

Run these commands:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Documentation is available as Unix manual pages in the `docs/` directory.

For detailed build instruction for various operating systems, see the
[Building from source](https://github.com/cutechess/cutechess/wiki/Building-from-source)
wiki page.

## Running

The `cutechess` program is the graphical user interface of Cute Chess.
It can be run either from command-line or from your desktop environment's
application launcher.

The `cutechess-cli` program is the command-line interface for playing
games between chess engines. For example, to play ten games between two Sloppy
engines (assuming `sloppy` is in PATH) with a time control of 40 moves in 60
seconds:

    $ cutechess-cli -engine cmd=sloppy -engine cmd=sloppy -each proto=xboard tc=40/60 -rounds 10

See `cutechess-cli -help` for descriptions of the supported options or manuals
for full documentation.

## License

Cute Chess is released under the GPLv3+ license except for the components in
the `projects/lib/components` and `projects/gui/components` directories which
are released under the MIT License.

## Credits

Cute Chess was written by Ilari Pihlajisto, Arto Jonsson and [contributors](https://github.com/cutechess/cutechess/graphs/contributors)
