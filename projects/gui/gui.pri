SLOPPYGUI_VERSION = unknown

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

macx-xcode {
    DEFINES += SLOPPYGUI_VERSION=\"$$SLOPPYGUI_VERSION\"
}else {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
    DEFINES += SLOPPYGUI_VERSION=\\\"$$SLOPPYGUI_VERSION\\\"
}

CONFIG += qt debug
QT += svg

win32 : debug {
        CONFIG += console
    }

# Components
include(components/hintlineedit/src/hintlineedit.pri)

# Base 
include(base/base.pri)

# GUI
include(gui/gui.pri)

# Forms
include(../ui/ui.pri)

UI_HEADERS_DIR = gui

# Resources
include(../res/res.pri)
