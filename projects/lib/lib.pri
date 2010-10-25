INCLUDEPATH += $$PWD/src
LIBS += -lcutechess -L$$PWD

win32:!static {
    DEFINES += LIB_EXPORT=\"__declspec(dllimport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}
