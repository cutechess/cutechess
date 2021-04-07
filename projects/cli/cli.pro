TARGET = cutechess-cli
DESTDIR = $$PWD

include(../lib/lib.pri)
include(../lib/libexport.pri)

CUTECHESS_CLI_VERSION = 1.2.0

macx-xcode {
    DEFINES += CUTECHESS_CLI_VERSION=\"$$CUTECHESS_CLI_VERSION\"
} else {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
    DEFINES += CUTECHESS_CLI_VERSION=\\\"$$CUTECHESS_CLI_VERSION\\\"
}

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
