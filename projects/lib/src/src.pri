include(chessboard/chessboard.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += chessengine.h \
           chessgame.h \
           chessplayer.h \
           engineconfiguration.h \
           pgngame.h \
           symbols.h \
           timecontrol.h \
           uciengine.h \
           xboardengine.h

SOURCES += chessengine.cpp \
           chessgame.cpp \
           chessplayer.cpp \
           engineconfiguration.cpp \
           pgngame.cpp \
           symbols.cpp \
           timecontrol.cpp \
           uciengine.cpp \
           xboardengine.cpp
