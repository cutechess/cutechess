INCLUDEPATH += $$PWD/src
LIBS += -lcutechess -L$$PWD

win32 {
    include($$PWD/3rdparty/qjson/qjson.pri)
} else {
    LIBS += -lqjson
}

win32:!static {
    DEFINES += LIB_EXPORT=\"__declspec(dllimport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}
