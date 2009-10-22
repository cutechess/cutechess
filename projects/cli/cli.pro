TEMPLATE = app
TARGET = cutechess-cli
DESTDIR = $$PWD

include(../lib/lib.pri)

win32 {
    DEFINES += LIB_EXPORT=\"__declspec(dllimport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

CUTECHESS_CLI_VERSION = 0.2.0

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

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

mac {
    CONFIG -= app_bundle
}

QT -= gui

# Code
include(src/src.pri)

