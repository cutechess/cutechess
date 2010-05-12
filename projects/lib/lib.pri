INCLUDEPATH += $$PWD/src
LIBS += -lchess -L$$PWD

exists($$PWD/3rdparty/qjson) {
    INCLUDEPATH += $$PWD/3rdparty/qjson/include
    LIBS += -L$$PWD/3rdparty/qjson/lib
}
LIBS += -lqjson

win32:!static {
    DEFINES += LIB_EXPORT=\"__declspec(dllimport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}
