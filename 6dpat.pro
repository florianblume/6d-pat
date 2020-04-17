#-------------------------------------------------
#
# Project created by QtCreator 2017-06-23T16:52:08
#
#-------------------------------------------------

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = src tests

tests.depends = src

OTHER_FILES += defaults.pri
