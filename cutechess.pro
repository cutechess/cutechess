TEMPLATE = subdirs
SUBDIRS = projects

# API documentation (Doxygen)
doc-api.commands = doxygen docs/api/api.doxygen
QMAKE_EXTRA_TARGETS += doc-api

# man documentation
unix:doc-man.commands = a2x -f manpage docs/man/cutechess.6.txt
unix:QMAKE_EXTRA_TARGETS += doc-man
