include(chessboard/chessboard.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += chessengine.h \
    chessgame.h \
    chessplayer.h \
    engineconfiguration.h \
    enginefactory.h \
    openingbook.h \
    pgnfile.h \
    pgngame.h \
    polyglotbook.h \
    timecontrol.h \
    uciengine.h \
    ucioption.h \
    xboardengine.h \
    moveevaluation.h \
    enginesettings.h \
    humanplayer.h
SOURCES += chessengine.cpp \
    chessgame.cpp \
    chessplayer.cpp \
    engineconfiguration.cpp \
    enginefactory.cpp \
    openingbook.cpp \
    pgnfile.cpp \
    pgngame.cpp \
    polyglotbook.cpp \
    timecontrol.cpp \
    uciengine.cpp \
    ucioption.cpp \
    xboardengine.cpp \
    moveevaluation.cpp \
    enginesettings.cpp \
    humanplayer.cpp
win32 { 
    HEADERS += engineprocess_win.h \
        pipereader_win.h
    SOURCES += engineprocess_win.cpp \
        pipereader_win.cpp
}
