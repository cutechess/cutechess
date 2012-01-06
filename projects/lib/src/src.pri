include(board/board.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += chessengine.h \
    chessgame.h \
    chessplayer.h \
    engineconfiguration.h \
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
    enginebuilder.h \
    classregistry.h \
    enginefactory.h \
    humanbuilder.h \
    engineoptionfactory.h \
    pgngamefilter.h \
    tournament.h \
    roundrobintournament.h \
    tournamentfactory.h \
    gauntlettournament.h
SOURCES += chessengine.cpp \
    chessgame.cpp \
    chessplayer.cpp \
    engineconfiguration.cpp \
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
    playerbuilder.cpp \
    enginebuilder.cpp \
    enginefactory.cpp \
    humanbuilder.cpp \
    engineoptionfactory.cpp \
    pgngamefilter.cpp \
    tournament.cpp \
    roundrobintournament.cpp \
    tournamentfactory.cpp \
    gauntlettournament.cpp
win32 { 
    HEADERS += engineprocess_win.h \
        pipereader_win.h
    SOURCES += engineprocess_win.cpp \
        pipereader_win.cpp
}


