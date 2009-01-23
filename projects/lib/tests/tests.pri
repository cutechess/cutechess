win32:config += CONSOLE

CONFIG += qtestlib

win32 {
    DEFINES += LIB_EXPORT=\"__declspec(dllexport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

include(../lib.pri)
