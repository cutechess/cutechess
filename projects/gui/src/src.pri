include(boardview/boardview.pri)
DEPENDPATH += $$PWD
HEADERS += chessclock.h \
    engineconfigurationmodel.h \
    engineconfigurationdlg.h \
    enginemanagementdlg.h \
    mainwindow.h \
    plaintextlog.h \
    newgamedlg.h \
    movelistmodel.h \
    cutechessapp.h \
    gamepropertiesdlg.h \
    autoverticalscroller.h \
    gamedatabasedlg.h \
    pgnimporter.h \
    gamedatabasemanager.h \
    importprogressdlg.h \
    pgndatabase.h \
    pgngameentrymodel.h \
    pgndatabasemodel.h
SOURCES += main.cpp \
    chessclock.cpp \
    engineconfigurationmodel.cpp \
    engineconfigurationdlg.cpp \
    enginemanagementdlg.cpp \
    mainwindow.cpp \
    plaintextlog.cpp \
    newgamedlg.cpp \
    movelistmodel.cpp \
    cutechessapp.cpp \
    gamepropertiesdlg.cpp \
    autoverticalscroller.cpp \
    gamedatabasedlg.cpp \
    pgnimporter.cpp \
    gamedatabasemanager.cpp \
    importprogressdlg.cpp \
    pgndatabase.cpp \
    pgngameentrymodel.cpp \
    pgndatabasemodel.cpp
