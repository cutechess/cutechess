Cute Chess
==========

Cute Chess is a graphical user interface, command-line interface and a library
for playing chess. Cute Chess is written in C++ using the [Qt
framework](http://qt-project.org/).

Installing
----------

Binaries are available for `cutechess-cli`, the command-line interface of Cute
Chess. The latest version is 0.6.0.

* [cutechess-cli GNU/Linux 32-bit](http://koti.mbnet.fi/~ilaripih/bin/cutechess-cli-linux32.tar.gz)
* [cutechess-cli GNU/Linux 64-bit](http://koti.mbnet.fi/~ilaripih/bin/cutechess-cli-linux64.tar.gz)
* [cutechess-cli OS X 64-bit](http://koti.mbnet.fi/~ilaripih/bin/cutechess-cli-osx.zip)
* [cutechess-cli Win32](http://koti.mbnet.fi/~ilaripih/bin/cutechess-cli-win32.zip)

The GUI doesn't have a binary release at this time.

Compiling
---------

Cute Chess requires Qt 4.6 or greater and `qmake`.

In the simplest case you only have to issue:

    $ qmake
    $ make

If you are using the Visual C++ compiler replace `make` with `nmake`. To build
on OS X add `-spec macx-g++` to the `qmake` command. To compile the
`libcutechess` library into the `cutechess` and `cutechess-cli` binaries add
`-config static` to the `qmake` command.

Documentation is available as Unix manual pages in the `docs/` directory.

API documentation can be built by issuing `make doc-api` (requires Doxygen).

Running
-------

In order to run the `cutechess` and `cutechess-cli` executables the program
loader must find the Qt libraries and the `libcutechess` library, if it's not
compiled statically. On Windows it's enough to copy the libraries (DLL files)
to the same directory as the Cute Chess executables. On Linux and OS X the path
to `libcutechess` should be set via an environment variable. On Linux the
command to run is:

    $ export LD_LIBRARY_PATH=path_to_libcutechess

And on OS X:

    $ export DYLD_LIBRARY_PATH=path_to_libcutechess

OS X users can also create a .dmg bundle of Cute Chess and the Qt frameworks
with the `macdeployqt` tool that is bundled with the Qt SDK.

License
-------

Cute Chess is released under the GPLv3+ license except for the components in
the `projects/lib/components` and `projects/gui/components` directories which
are released under the MIT License.

Credits
-------

Cute Chess was written by Ilari Pihlajisto and Arto Jonsson.
