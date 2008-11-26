include(chessboard/chessboard.pri)

DEPENDPATH += $$PWD

HEADERS += chessengine.h \
           chessgame.h \
           chessplayer.h \
           engineconfiguration.h \
           engineconfigurationmodel.h \
           loggerbase.h \
           logmanager.h \
           manager.h \
           pgngame.h \
           stdoutlogger.h \
           symbols.h \
           timecontrol.h \
           uciengine.h \
           xboardengine.h

SOURCES += chessengine.cpp \
           chessgame.cpp \
           chessplayer.cpp \
           engineconfiguration.cpp \
           engineconfigurationmodel.cpp \
           logmanager.cpp \
           manager.cpp \
           pgngame.cpp \
           stdoutlogger.cpp \
           symbols.cpp \
           timecontrol.cpp \
           uciengine.cpp \
           xboardengine.cpp
