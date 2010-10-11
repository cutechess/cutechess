INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += json_scanner.h \
    parser_p.h \
    qjson_debug.h \
    json_parser.hh \
    location.hh \
    position.hh \
    stack.hh

SOURCES += json_scanner.cpp \
    parser.cpp \
    parserrunnable.cpp \
    qobjecthelper.cpp \
    serializer.cpp \
    serializerrunnable.cpp \
    json_parser.cc
