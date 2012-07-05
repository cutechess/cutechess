win32:config += CONSOLE
CONFIG += qtestlib
DEFINES += LIB_EXPORT=""

include(../src/json.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
