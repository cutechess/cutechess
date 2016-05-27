Cute Chess
==========

[![Build Status](https://travis-ci.org/cutechess/cutechess.svg?branch=master)](https://travis-ci.org/cutechess/cutechess) [![Coverity Scan Build Status](https://scan.coverity.com/projects/8561/badge.svg)](https://scan.coverity.com/projects/cutechess-cutechess)

Cute Chess is a graphical user interface, command-line interface and a library
for playing chess. Cute Chess is written in C++ using the [Qt
framework](https://www.qt.io/).

Installing
----------

Binaries are available for `cutechess-cli`, the command-line interface of Cute
Chess. The latest version is 0.8.1.

* [cutechess-cli GNU/Linux 64-bit](http://ilaripih.mbnet.fi/bin/cutechess-cli-linux64.tar.gz)
* [cutechess-cli OS X 64-bit](http://ilaripih.mbnet.fi/bin/cutechess-cli-osx.zip)
* [cutechess-cli Win64](http://ilaripih.mbnet.fi/bin/cutechess-cli-win64.zip)

The GUI doesn't have a binary release at this time.

Compiling
---------

Cute Chess requires Qt 5.3 or greater, a compiler with C++11 support and `qmake`.

In the simplest case you only have to issue:

    $ qmake
    $ make

If you are using the Visual C++ compiler replace `make` with `nmake`.

Documentation is available as Unix manual pages in the `docs/` directory. API
documentation can be built by issuing `make doc-api` (requires Doxygen).

Running
-------

The `cutechess-cli` program is run from the command line to play games between
chess engines. For example to play ten games between two Sloppy engines
(assuming `sloppy` is in PATH) with a time control of 40 moves in 60
seconds:

    $ cutechess-cli -engine cmd=sloppy -engine cmd=sloppy -each proto=xboard tc=40/60 -rounds 10

See `cutechess-cli -help` for descriptions of the supported options or manuals
for full documentation.

License
-------

Cute Chess is released under the GPLv3+ license except for the components in
the `projects/lib/components` and `projects/gui/components` directories which
are released under the MIT License.

Credits
-------

Cute Chess was written by Ilari Pihlajisto and Arto Jonsson.
