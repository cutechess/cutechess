include(chessboard/chessboard.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += bookmove.h \
           chessengine.h \
           chessgame.h \
           chessplayer.h \
           engineconfiguration.h \
           openingbook.h \
           pgngame.h \
           polyglotbook.h \
           symbols.h \
           timecontrol.h \
           uciengine.h \
           xboardengine.h

SOURCES += bookmove.cpp \
           chessengine.cpp \
           chessgame.cpp \
           chessplayer.cpp \
           engineconfiguration.cpp \
           openingbook.cpp \
           pgngame.cpp \
           polyglotbook.cpp \
           symbols.cpp \
           timecontrol.cpp \
           uciengine.cpp \
           xboardengine.cpp
