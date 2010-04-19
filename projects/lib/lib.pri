INCLUDEPATH += $$PWD/src
LIBS += -lchess -lqjson -L$$PWD

exists($$PWD/3rdparty/qjson) {
    INCLUDEPATH += $$PWD/3rdparty/qjson/include
    LIBS += -L$$PWD/3rdparty/qjson/lib
}
