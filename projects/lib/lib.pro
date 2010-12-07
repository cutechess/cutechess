TEMPLATE = lib
TARGET = cutechess
QT = core
DESTDIR = $$PWD

win32:!static {
    DEFINES += LIB_EXPORT="__declspec(dllexport)"
} else {
    DEFINES += LIB_EXPORT=""
}

include(src/src.pri)
include(components/json/src/json.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
