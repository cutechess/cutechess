macx {
    TARGET = "Cute Chess"
} else {
    TARGET = cutechess
}

DESTDIR = $$PWD

include(../lib/lib.pri)
include(../lib/libexport.pri)

CUTECHESS_VERSION = unknown

macx-xcode {
    DEFINES += CUTECHESS_VERSION=\"$$CUTECHESS_VERSION\"
}else {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
    DEFINES += CUTECHESS_VERSION=\\\"$$CUTECHESS_VERSION\\\"
}

QT += svg widgets concurrent

win32 {
    CONFIG(debug, debug|release) {
	CONFIG += console
    }
    RC_FILE = res/icons/cutechess_win.rc
}

macx {
    ICON = res/icons/cutechess_mac.icns
}

# Components
include(components/hintlineedit/src/hintlineedit.pri)

# GUI
include(src/src.pri)

# Forms
include(ui/ui.pri)

UI_HEADERS_DIR = src

# Resources
include(res/res.pri)
