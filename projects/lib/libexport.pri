CONFIG += staticlib

win32:!CONFIG(staticlib) {
    DEFINES += LIB_EXPORT="__declspec(dllexport)"
} else {
    DEFINES += LIB_EXPORT=""
}
