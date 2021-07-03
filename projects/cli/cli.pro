TARGET = cutechess-cli
DESTDIR = $$PWD

include(../../version.pri)

include(../lib/lib.pri)
include(../lib/libexport.pri)

OBJECTS_DIR = .obj/
MOC_DIR = .moc/
RCC_DIR = .rcc/

win32 {
    CONFIG += console
}

!win32-msvc* {
	QMAKE_CXXFLAGS += -Wextra
}

mac {
    CONFIG -= app_bundle
}

QT = core
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += core5compat
}

# Code
include(src/src.pri)

# Resources
include(res/res.pri)
