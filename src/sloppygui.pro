CONFIG += qt debug
QT += svg

win32:debug {
	CONFIG += console
}

HEADERS = chessboardsquareitem.h \
          chessboarditem.h \
          chessboardwidget.h \
          manager.h \
          logmanager.h \
          stdoutlogger.h \
          guilogger.h \
          loggerbase.h \
          mainwindow.h

SOURCES = chessboardsquareitem.cpp \
          chessboarditem.cpp \
          chessboardwidget.cpp \
          manager.cpp \
          logmanager.cpp \
          stdoutlogger.cpp \
          guilogger.cpp \
          mainwindow.cpp \
          main.cpp

OBJECTS_DIR = .obj/
MOC_DIR = .moc/
