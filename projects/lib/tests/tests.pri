TEMPLATE = app

win32:config += CONSOLE

mac {
	CONFIG -= app_bundle
}

QT = core testlib
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += core5compat
}
CONFIG += testcase

include(../lib.pri)
include(../libexport.pri)

OBJECTS_DIR = .obj
MOC_DIR = .moc
