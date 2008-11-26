TEMPLATE = lib
TARGET = hintlineedit
TARGET = $$qtLibraryTarget($$TARGET)
CONFIG += designer plugin
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/designer

include(../src/hintlineedit.pri)

HEADERS += hintlineeditplugin.h

SOURCES += hintlineeditplugin.cpp

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
