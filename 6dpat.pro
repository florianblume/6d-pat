TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = src tests

tests.depends = src
