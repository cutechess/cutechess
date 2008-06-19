CONFIG += qt debug
QT += svg

win32:debug {
	CONFIG += console
}

HEADERS = graphicschessboardsquareitem.h \
          graphicschessboarditem.h \
          chessboardview.h \
          manager.h \
          logmanager.h \
          stdoutlogger.h \
          guilogger.h \
          loggerbase.h \
          mainwindow.h

SOURCES = graphicschessboardsquareitem.cpp \
          graphicschessboarditem.cpp \
          chessboardview.cpp \
          manager.cpp \
          logmanager.cpp \
          stdoutlogger.cpp \
          guilogger.cpp \
          mainwindow.cpp \
          main.cpp

OBJECTS_DIR = .obj/
MOC_DIR = .moc/
