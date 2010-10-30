win32:config += CONSOLE

CONFIG += qtestlib

win32 {
    DEFINES += LIB_EXPORT=\"__declspec(dllimport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

include(../src/json.pri)
