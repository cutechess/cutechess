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
          src/mainwindow.h

SOURCES = src/graphicschessboardsquareitem.cpp \
          src/graphicschessboarditem.cpp \
          src/chessboardview.cpp \
          src/manager.cpp \
          src/logmanager.cpp \
          src/stdoutlogger.cpp \
          src/guilogger.cpp \
          src/mainwindow.cpp \
          src/main.cpp

OBJECTS_DIR = .obj/
MOC_DIR = .moc/
