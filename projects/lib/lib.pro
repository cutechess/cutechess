TEMPLATE = lib
TARGET = chess
QT = core
DESTDIR = $$PWD

win32 {
    DEFINES += LIB_EXPORT=\"__declspec(dllexport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

exists($$PWD/3rdparty/qjson) {
    INCLUDEPATH += $$PWD/3rdparty/qjson/include
    LIBS += -L$$PWD/3rdparty/qjson/lib
}
LIBS += -lqjson

include(src/src.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
