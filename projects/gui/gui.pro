macx {
    TARGET = "Cute Chess"
} else {
    TARGET = cutechess
}

DESTDIR = $$PWD

include(../lib/lib.pri)
include(../lib/libexport.pri)

CUTECHESS_VERSION = 1.1.0

macx-xcode {
    DEFINES += CUTECHESS_VERSION=\"$$CUTECHESS_VERSION\"
}else {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
    DEFINES += CUTECHESS_VERSION=\\\"$$CUTECHESS_VERSION\\\"
}

QT += svg widgets concurrent printsupport

win32 {
    CONFIG(debug, debug|release) {
	CONFIG += console
    }
    RC_FILE = res/icons/cutechess_win.rc
}

!win32-msvc* {
	QMAKE_CXXFLAGS += -Wextra -Wshadow
}

macx {
    ICON = res/icons/cutechess_mac.icns
}

UI_HEADERS_DIR = src

include(src/src.pri)
include(ui/ui.pri)
include(res/res.pri)
include(3rdparty/qcustomplot/qcustomplot.pri)

CONFIG(debug, debug|release) {
    include(3rdparty/modeltest/modeltest.pri)
}
