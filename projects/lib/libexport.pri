CONFIG += c++11

# Static linking is used by default.
# Set "CONFIG+=dynamic" to use dynamic linking
VAL_LIB_EXPORT = ""
equals(TEMPLATE, "lib") {
    dynamic {
	win32 {
	    VAL_LIB_EXPORT="__declspec(dllexport)"
	}
    } else {
	CONFIG += staticlib
    }
} else:win32:dynamic {
    VAL_LIB_EXPORT="__declspec(dllimport)"
}
DEFINES += LIB_EXPORT=$$VAL_LIB_EXPORT
