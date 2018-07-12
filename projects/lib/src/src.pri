include(board/board.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += $$PWD/chessengine.h \
    $$PWD/chessgame.h \
    $$PWD/chessplayer.h \
    $$PWD/engineconfiguration.h \
    $$PWD/openingbook.h \
    $$PWD/pgnstream.h \
    $$PWD/pgngame.h \
    $$PWD/polyglotbook.h \
    $$PWD/timecontrol.h \
    $$PWD/uciengine.h \
    $$PWD/xboardengine.h \
    $$PWD/moveevaluation.h \
    $$PWD/enginemanager.h \
    $$PWD/humanplayer.h \
    $$PWD/engineoption.h \
    $$PWD/enginespinoption.h \
    $$PWD/enginecombooption.h \
    $$PWD/enginecheckoption.h \
    $$PWD/enginetextoption.h \
    $$PWD/enginebuttonoption.h \
    $$PWD/pgngameentry.h \
    $$PWD/gamemanager.h \
    $$PWD/playerbuilder.h \
    $$PWD/enginebuilder.h \
    $$PWD/classregistry.h \
    $$PWD/enginefactory.h \
    $$PWD/humanbuilder.h \
    $$PWD/engineoptionfactory.h \
    $$PWD/pgngamefilter.h \
    $$PWD/tournament.h \
    $$PWD/roundrobintournament.h \
    $$PWD/tournamentfactory.h \
    $$PWD/gauntlettournament.h \
    $$PWD/epdrecord.h \
    $$PWD/openingsuite.h \
    $$PWD/econode.h \
    $$PWD/mersenne.h \
    $$PWD/sprt.h \
    $$PWD/gameadjudicator.h \
    $$PWD/elo.h \
    $$PWD/knockouttournament.h \
    $$PWD/pyramidtournament.h \
    $$PWD/tournamentplayer.h \
    $$PWD/tournamentpair.h \
    $$PWD/worker.h
SOURCES += $$PWD/chessengine.cpp \
    $$PWD/chessgame.cpp \
    $$PWD/chessplayer.cpp \
    $$PWD/engineconfiguration.cpp \
    $$PWD/openingbook.cpp \
    $$PWD/pgnstream.cpp \
    $$PWD/pgngame.cpp \
    $$PWD/polyglotbook.cpp \
    $$PWD/timecontrol.cpp \
    $$PWD/uciengine.cpp \
    $$PWD/xboardengine.cpp \
    $$PWD/moveevaluation.cpp \
    $$PWD/enginemanager.cpp \
    $$PWD/humanplayer.cpp \
    $$PWD/engineoption.cpp \
    $$PWD/enginespinoption.cpp \
    $$PWD/enginecombooption.cpp \
    $$PWD/enginecheckoption.cpp \
    $$PWD/enginetextoption.cpp \
    $$PWD/enginebuttonoption.cpp \
    $$PWD/pgngameentry.cpp \
    $$PWD/gamemanager.cpp \
    $$PWD/playerbuilder.cpp \
    $$PWD/enginebuilder.cpp \
    $$PWD/enginefactory.cpp \
    $$PWD/humanbuilder.cpp \
    $$PWD/engineoptionfactory.cpp \
    $$PWD/pgngamefilter.cpp \
    $$PWD/tournament.cpp \
    $$PWD/roundrobintournament.cpp \
    $$PWD/tournamentfactory.cpp \
    $$PWD/gauntlettournament.cpp \
    $$PWD/epdrecord.cpp \
    $$PWD/openingsuite.cpp \
    $$PWD/econode.cpp \
    $$PWD/mersenne.cpp \
    $$PWD/sprt.cpp \
    $$PWD/gameadjudicator.cpp \
    $$PWD/elo.cpp \
    $$PWD/knockouttournament.cpp \
    $$PWD/pyramidtournament.cpp \
    $$PWD/tournamentplayer.cpp \
    $$PWD/tournamentpair.cpp \
    $$PWD/worker.cpp
win32 { 
    HEADERS += $$PWD/engineprocess_win.h \
	$$PWD/pipereader_win.h
    SOURCES += $$PWD/engineprocess_win.cpp \
	$$PWD/pipereader_win.cpp
}
