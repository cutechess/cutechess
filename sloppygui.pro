SLOPPYGUI_VERSION = unknown

# Check if the version file exists.
exists(version){

    # If the version file exists, use its contents as the version number
    SLOPPYGUI_VERSION = $$system(cat version)
}else {

    # If the version file doesn't exist, check if we have
    # .git directory
    exists(.git){

        # Check if we can describe this commit
        system(git describe > /dev/null 2> /dev/null){

            # If we can, describe it
            SLOPPYGUI_VERSION = $$system(git describe)
        }        else {

            # If we can't describe it, parse the sha id and use that
            SLOPPYGUI_VERSION = git-$$system(git rev-parse --short HEAD)
        }
    }
}

macx-xcode {
    DEFINES += SLOPPYGUI_VERSION=\"$$SLOPPYGUI_VERSION\"
}else {
    DEFINES += SLOPPYGUI_VERSION=\\\"$$SLOPPYGUI_VERSION\\\"
}

CONFIG += qt debug
QT += svg

win32 : debug {
        CONFIG += console
    }

# Components
include(src/components/hintlineedit/src/hintlineedit.pri)

HEADERS += src/graphicschessboardsquareitem.h \
           src/graphicschessboarditem.h \
           src/graphicschesspiece.h \
           src/chessboardview.h \
           src/chessclock.h \
           src/manager.h \
           src/logmanager.h \
           src/newgamedlg.h \
           src/enginemanagementdlg.h \
           src/engineconfigurationdlg.h \
           src/stdoutlogger.h \
           src/guilogger.h \
           src/loggerbase.h \
           src/mainwindow.h \
           src/symbols.h \
           src/promotiondlg.h \
           src/chessgame.h \
           src/timecontrol.h \
           src/chessplayer.h \
           src/chessengine.h \
           src/engineconfiguration.h \
           src/engineconfigurationmodel.h \
           src/xboardengine.h \
           src/uciengine.h \
           src/chessboard/archbishop.h \
           src/chessboard/fen.h \
           src/chessboard/bishop.h \
           src/chessboard/king.h \
           src/chessboard/capablancachessboard.h \
           src/chessboard/knight.h \
           src/chessboard/castlingrights.h \
           src/chessboard/movenotation.h \
           src/chessboard/chancellor.h \
           src/chessboard/pawn.h \
           src/chessboard/chessboard.h \
           src/chessboard/queen.h \
           src/chessboard/chessmove.h \
           src/chessboard/rook.h \
           src/chessboard/chesspiece.h \
           src/chessboard/standardchessboard.h \
           src/chessboard/completechessmove.h \
           src/chessboard/zobrist.h

SOURCES += src/main.cpp \
           src/graphicschessboardsquareitem.cpp \
           src/graphicschessboarditem.cpp \
           src/graphicschesspiece.cpp \
           src/chessboardview.cpp \
           src/chessclock.cpp \
           src/manager.cpp \
           src/logmanager.cpp \
           src/stdoutlogger.cpp \
           src/guilogger.cpp \
           src/mainwindow.cpp \
           src/symbols.cpp \
           src/promotiondlg.cpp \
           src/chessgame.cpp \
           src/timecontrol.cpp \
           src/chessplayer.cpp \
           src/chessengine.cpp \
           src/engineconfiguration.cpp \
           src/engineconfigurationmodel.cpp \
           src/xboardengine.cpp \
           src/uciengine.cpp \
           src/newgamedlg.cpp \
           src/enginemanagementdlg.cpp \
           src/engineconfigurationdlg.cpp \
           src/chessboard/archbishop.cpp \
           src/chessboard/fen.cpp \
           src/chessboard/bishop.cpp \
           src/chessboard/king.cpp \
           src/chessboard/capablancachessboard.cpp \
           src/chessboard/knight.cpp \
           src/chessboard/castlingrights.cpp \
           src/chessboard/movenotation.cpp \
           src/chessboard/chancellor.cpp \
           src/chessboard/pawn.cpp \
           src/chessboard/chessboard.cpp \
           src/chessboard/queen.cpp \
           src/chessboard/chessmove.cpp \
           src/chessboard/rook.cpp \
           src/chessboard/chesspiece.cpp \
           src/chessboard/standardchessboard.cpp \
           src/chessboard/completechessmove.cpp \
           src/chessboard/zobrist.cpp

FORMS += ui/newgamedlg.ui \
         ui/enginemanagementdlg.ui \
         ui/engineconfigdlg.ui

UI_HEADERS_DIR = src

RESOURCES += res/chessboard/chessboard.qrc

!macx-xcode {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
}

# API documentation (Doxygen)
doc-api.commands = doxygen docs/api/api.doxygen
QMAKE_EXTRA_TARGETS += doc-api

# man documentation
unix : doc-man.commands = a2x -f manpage docs/man/sloppygui.6.txt
unix : QMAKE_EXTRA_TARGETS += doc-man

