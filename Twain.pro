QT       += core

TARGET = $$qtLibraryTarget(QTwain)
TEMPLATE = lib
VERSION = 1.0.1
DESTDIR = dist
CONFIG += build_all dll

DEFINES += QTwain_LIBRARY

SOURCES += \
    twaincpp.cpp \
    qtwain.cpp \
    dib.cpp \
    dibutil.c \
    dibfile.c

HEADERS += \
    twaincpp.h \
    twain.h \
    qtwain.h \
    dibutil.h \
    dibapi.h \
    dib.h \
    qtwain_global.h


LIBS += -lgdi32
