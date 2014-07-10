include(../tests.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += concurrent
TARGET = tst_board
SOURCES += tst_board.cpp
