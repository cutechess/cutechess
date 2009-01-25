TEMPLATE = app
TARGET = cutechess
DESTDIR = $$PWD

include(../lib/lib.pri)

win32 {
    DEFINES += LIB_EXPORT=\"__declspec(dllimport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

CUTECHESS_VERSION = unknown

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

macx-xcode {
    DEFINES += CUTECHESS_VERSION=\"$$CUTECHESS_VERSION\"
}else {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
    DEFINES += CUTECHESS_VERSION=\\\"$$CUTECHESS_VERSION\\\"
}

QT += svg

win32 : debug {
        CONFIG += console
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
