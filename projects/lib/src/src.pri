include(chessboard/chessboard.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += chessengine.h \
    chessgame.h \
    chessplayer.h \
    engineconfiguration.h \
    openingbook.h \
    pgnfile.h \
    pgngame.h \
    polyglotbook.h \
    symbols.h \
    timecontrol.h \
    uciengine.h \
    ucioption.h \
    xboardengine.h
SOURCES += chessengine.cpp \
    chessgame.cpp \
    chessplayer.cpp \
    engineconfiguration.cpp \
    openingbook.cpp \
    pgnfile.cpp \
    pgngame.cpp \
    polyglotbook.cpp \
    symbols.cpp \
    timecontrol.cpp \
    uciengine.cpp \
    ucioption.cpp \
    xboardengine.cpp
