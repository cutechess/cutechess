CONFIG += ordered

TEMPLATE = subdirs
SUBDIRS = lib gui cli

cli.depends = lib
gui.depends = lib
