TEMPLATE = app
TARGET = test_board

win32 {
    DEFINES += LIB_EXPORT=\"__declspec(dllexport)\"
} else {
    DEFINES += LIB_EXPORT=\"\"
}

include(../tests.pri)

SOURCES += test_board.cpp

OBJECTS_DIR = .obj
MOC_DIR = .moc
