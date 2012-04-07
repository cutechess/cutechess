Cute Chess
==========

Cute Chess is a graphical user interface, command-line interface and a library
for playing chess. Cute Chess is written in C++ using the [Qt
framework](http://qt.nokia.com/).

Installing
----------

Binaries are available for `cutechess-cli`. The latest version is 0.5.1.

[cutechess-cli GNU/Linux 32-bit](http://github.com/downloads/cutechess/cutechess/cutechess-cli-linux32.tar.gz)
[cutechess-cli GNU/Linux 64-bit](http://github.com/downloads/cutechess/cutechess/cutechess-cli-linux64.tar.gz)
[cutechess-cli OS X 64-bit](http://github.com/downloads/cutechess/cutechess/cutechess-cli-osx.zip)
[cutechess-cli Win32](http://github.com/downloads/cutechess/cutechess/cutechess-cli-win32.zip)

The GUI doesn't have a binary release at this time.

Compiling
---------

Cute Chess requires Qt 4.6 or greater and `qmake`.

In the simplest case you only have to issue:

    $ qmake
    $ make

If you are using the Visual C++ compiler replace `make` with `nmake`. To build
on OS X add `-spec macx-g++` to the `qmake` command.

Documentation is available as Unix manual pages in the `docs/` directory.

API documentation can be built by issuing `make doc-api` (requires Doxygen).

Credits
-------

Cute Chess was written by Ilari Pihlajisto and Arto Jonsson.
