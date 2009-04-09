TEMPLATE = subdirs
SUBDIRS = projects

# API documentation (Doxygen)
doc-api.commands = doxygen docs/api/api.doxygen
QMAKE_EXTRA_TARGETS += doc-api

# man documentation
unix:doc-man.commands += a2x -f manpage docs/cutechess.txt;
unix:doc-man.commands += a2x -f manpage docs/cutechess-cli.txt
unix:QMAKE_EXTRA_TARGETS += doc-man

# html documentation
unix:doc-html.commands += a2x -f xhtml docs/cutechess.txt;
unix:doc-html.commands += a2x -f xhtml docs/cutechess-cli.txt
unix:QMAKE_EXTRA_TARGETS += doc-html
