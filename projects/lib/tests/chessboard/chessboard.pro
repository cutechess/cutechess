TEMPLATE = app
include(../tests.pri)

TARGET = test_board
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

include($$SRC_PATH/chessboard/chessboard.pri)

# Input
SOURCES += test_board.cpp
