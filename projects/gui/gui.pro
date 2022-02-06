macx {
    TARGET = "Cute Chess"
} else {
    TARGET = cutechess
}

DESTDIR = $$PWD

include(../../version.pri)

include(../lib/lib.pri)
include(../lib/libexport.pri)

OBJECTS_DIR = .obj/
MOC_DIR = .moc/
RCC_DIR = .rcc/

QT += svg widgets concurrent printsupport charts
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += core5compat
}

win32 {
    CONFIG(debug, debug|release) {
	CONFIG += console
    }
    RC_FILE = res/icons/cutechess_win.rc
}

!win32-msvc* {
	QMAKE_CXXFLAGS += -Wextra
}

macx {
    ICON = res/icons/cutechess_mac.icns
}

UI_HEADERS_DIR = src

include(src/src.pri)
include(ui/ui.pri)
include(res/res.pri)

CONFIG(debug, debug|release) {
    include(3rdparty/modeltest/modeltest.pri)
}
