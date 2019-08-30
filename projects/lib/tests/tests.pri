TEMPLATE = app

win32:config += CONSOLE

mac {
	CONFIG -= app_bundle
}

QT = core testlib
CONFIG += testcase

include(../lib.pri)
include(../libexport.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
