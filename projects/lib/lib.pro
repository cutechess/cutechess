TEMPLATE = lib
TARGET = chess
QT = core
DESTDIR = $$PWD

win32 {
    DEFINES += LIB_EXPORT=\"__declspec(dllexport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

include(src/src.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
