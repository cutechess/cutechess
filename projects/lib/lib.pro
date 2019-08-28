TEMPLATE = lib
TARGET = cutechess
QT = core
DESTDIR = $$PWD

!win32-msvc* {
	QMAKE_CXXFLAGS += -Wextra
}

include(libexport.pri)
include(src/src.pri)
include(components/json/src/json.pri)
include(3rdparty/fathom/src/tb.pri)
include(res/res.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
