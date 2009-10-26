include(chessboard/chessboard.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += chessengine.h \
    chessgame.h \
    chessplayer.h \
    engineconfiguration.h \
    enginefactory.h \
    openingbook.h \
    pgnstream.h \
    pgngame.h \
    polyglotbook.h \
    timecontrol.h \
    uciengine.h \
    xboardengine.h \
    moveevaluation.h \
    enginemanager.h \
    humanplayer.h \
    engineoption.h \
    enginespinoption.h \
    enginecombooption.h \
    enginecheckoption.h \
    enginetextoption.h \
    enginebuttonoption.h \
    pgngameentry.h \
    gamemanager.h \
    playerbuilder.h \
    enginebuilder.h
SOURCES += chessengine.cpp \
    chessgame.cpp \
    chessplayer.cpp \
    engineconfiguration.cpp \
    enginefactory.cpp \
    openingbook.cpp \
    pgnstream.cpp \
    pgngame.cpp \
    polyglotbook.cpp \
    timecontrol.cpp \
    uciengine.cpp \
    xboardengine.cpp \
    moveevaluation.cpp \
    enginemanager.cpp \
    humanplayer.cpp \
    engineoption.cpp \
    enginespinoption.cpp \
    enginecombooption.cpp \
    enginecheckoption.cpp \
    enginetextoption.cpp \
    enginebuttonoption.cpp \
    pgngameentry.cpp \
    gamemanager.cpp \
    enginebuilder.cpp
win32 { 
    HEADERS += engineprocess_win.h \
        pipereader_win.h
    SOURCES += engineprocess_win.cpp \
        pipereader_win.cpp
}
