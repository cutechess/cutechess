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
doc-html.commands += a2x -f xhtml docs/cutechess.txt;
doc-html.commands += a2x -f xhtml docs/cutechess-cli.txt
QMAKE_EXTRA_TARGETS += doc-html
