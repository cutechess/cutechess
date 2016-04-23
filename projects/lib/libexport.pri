CONFIG += staticlib
CONFIG += c++11

win32:!CONFIG(staticlib) {
    equals(TEMPLATE, "lib") {
	DEFINES += LIB_EXPORT="__declspec(dllexport)"
    } else {
	DEFINES += LIB_EXPORT="__declspec(dllimport)"
    }
} else {
    DEFINES += LIB_EXPORT=""
}
