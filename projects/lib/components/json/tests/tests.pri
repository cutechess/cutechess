win32:config += CONSOLE
QT = core testlib
DEFINES += LIB_EXPORT=""

include(../src/json.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
