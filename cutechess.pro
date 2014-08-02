# Check Qt version
contains(QT_VERSION, ^4\\.[0-6]\\..*) {
    message("Cannot build Cute Chess with Qt version $${QT_VERSION}.")
    error("Qt version 4.7 or later is required.")
}

TEMPLATE = subdirs
SUBDIRS = projects

# API documentation (Doxygen)
doc-api.commands = doxygen docs/api/api.doxygen
QMAKE_EXTRA_TARGETS += doc-api

# Documentation in HTML format
doc-html.commands = \
    mandoc -Txhtml -Ostyle=man-style.css docs/cutechess-cli.6 > docs/cutechess-cli.6.html; \
    mandoc -Txhtml -Ostyle=man-style.css docs/engines.json.5 > docs/engines.json.5.html
QMAKE_EXTRA_TARGETS += doc-html
QMAKE_DISTCLEAN += docs/*.html

# Documentation in text format
doc-txt.commands = \
    mandoc -Tascii docs/cutechess-cli.6 | col -b > docs/cutechess-cli.6.txt; \
    mandoc -Tascii docs/engines.json.5 | col -b > docs/engines.json.5.txt
QMAKE_EXTRA_TARGETS += doc-txt
QMAKE_DISTCLEAN += docs/*.txt
