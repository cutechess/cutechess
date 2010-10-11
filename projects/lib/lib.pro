TEMPLATE = lib
TARGET = cutechess
QT = core
DESTDIR = $$PWD

win32:!static {
    DEFINES += LIB_EXPORT=\"__declspec(dllexport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

win32 {
    include($$PWD/3rdparty/qjson/qjson.pri)
} else {
    LIBS += -lqjson
}

include(src/src.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
