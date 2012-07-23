TEMPLATE = lib
TARGET = cutechess
QT = core

win32:!static {
    DEFINES += LIB_EXPORT="__declspec(dllexport)"
} else {
    DEFINES += LIB_EXPORT=""
}

include(src/src.pri)
include(components/json/src/json.pri)
include(res/res.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
