# Check Qt version
contains(QT_VERSION, ^4\.[0-5]\..*) {
    message("Cannot build Cute Chess with Qt version $${QT_VERSION}.")
    error("Qt version 4.6 or later is required.")
}

TEMPLATE = subdirs
SUBDIRS = projects

# API documentation (Doxygen)
doc-api.commands = doxygen docs/api/api.doxygen
QMAKE_EXTRA_TARGETS += doc-api

# Unix manual pages
doc-man.commands += a2x -f manpage docs/cutechess.txt;
doc-man.commands += a2x -f manpage docs/cutechess-cli.txt
QMAKE_EXTRA_TARGETS += doc-man

# html documentation
doc-html.commands += asciidoc -b xhtml11 docs/cutechess.txt;
doc-html.commands += asciidoc -b xhtml11 docs/cutechess-cli.txt
QMAKE_EXTRA_TARGETS += doc-html
