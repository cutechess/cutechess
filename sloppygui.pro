SLOPPYGUI_VERSION = unknown

# Check if the version file exists.
exists(version) {

	# If the version file exists, use its contents as the version number
	SLOPPYGUI_VERSION = $$system(cat version)
} else {

	# If the version file doesn't exist, check if we have
	# .git directory
	exists(.git) {

		# Check if we can describe this commit
		system(git describe > /dev/null 2> /dev/null) {

			# If we can, describe it
			SLOPPYGUI_VERSION = $$system(git describe)
		} else {

			# If we can't describe it, parse the sha id and use that
			SLOPPYGUI_VERSION = git-$$system(git rev-parse --short HEAD)
		}
	}
}

DEFINES += SLOPPYGUI_VERSION='\'"$$SLOPPYGUI_VERSION"\''

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
          src/symbols.h \
          src/promotiondlg.h \
          src/chessgame.h \
          src/chessplayer.h \
          src/chessengine.h \
          src/xboardengine.h \
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
          src/symbols.cpp \
          src/promotiondlg.cpp \
          src/main.cpp \
          src/chessgame.cpp \
          src/chessplayer.cpp \
          src/chessengine.cpp \
          src/xboardengine.cpp \
          src/chessboard/chessboard.cpp \
          src/chessboard/chessmove.cpp \
          src/chessboard/magicmoves.cpp \
          src/chessboard/makemove.cpp \
          src/chessboard/movegen.cpp \
          src/chessboard/notation.cpp \
          src/chessboard/util.cpp \
          src/chessboard/zobrist.cpp

RESOURCES = res/chessboard/chessboard.qrc

OBJECTS_DIR = .obj/
MOC_DIR = .moc/
RCC_DIR = .rcc/

# API documentation (Doxygen)
doc-api.commands = doxygen docs/api/api.doxygen
QMAKE_EXTRA_TARGETS += doc-api

# man documentation
unix:doc-man.commands = a2x -f manpage docs/man/sloppygui.6.txt
unix:QMAKE_EXTRA_TARGETS += doc-man

