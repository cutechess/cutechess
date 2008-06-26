CONFIG += qt debug
QT += svg

win32:debug {
	CONFIG += console
}

HEADERS = src/graphicschessboardsquareitem.h \
          src/graphicschessboarditem.h \
          src/chessboardview.h \
          src/manager.h \
          src/logmanager.h \
          src/stdoutlogger.h \
          src/guilogger.h \
          src/loggerbase.h \
          src/mainwindow.h \
          src/chessboard/chessboard.h \
          src/chessboard/chessmove.h \
          src/chessboard/magicmoves.h \
          src/chessboard/movegen.h \
          src/chessboard/notation.h \
          src/chessboard/util.h \
          src/chessboard/zobrist.h

SOURCES = src/graphicschessboardsquareitem.cpp \
          src/graphicschessboarditem.cpp \
          src/chessboardview.cpp \
          src/manager.cpp \
          src/logmanager.cpp \
          src/stdoutlogger.cpp \
          src/guilogger.cpp \
          src/mainwindow.cpp \
          src/main.cpp \
          src/chessboard/chessboard.cpp \
          src/chessboard/chessmove.cpp \
          src/chessboard/magicmoves.cpp \
          src/chessboard/makemove.cpp \
          src/chessboard/movegen.cpp \
          src/chessboard/notation.cpp \
          src/chessboard/util.cpp \
          src/chessboard/zobrist.cpp

RESOURCES = res/chessboard/chessboard.qrc

# API documentation (Doxygen)
docs-api.target = documentation-api
docs-api.commands = doxygen api.doxygen

QMAKE_EXTRA_TARGETS += docs-api

OBJECTS_DIR = .obj/
MOC_DIR = .moc/
RCC_DIR = .rcc/
