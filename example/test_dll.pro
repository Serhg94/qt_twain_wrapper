#-------------------------------------------------
#
# Project created by QtCreator 2016-05-27T17:52:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_dll
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-Twain-Desktop_Qt_5_5_0_MinGW_32bit-Release/dist/ -lQTwain1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-Twain-Desktop_Qt_5_5_0_MinGW_32bit-Release/dist/ -lQTwain1d
else:unix: LIBS += -L$$PWD/../build-Twain-Desktop_Qt_5_5_0_MinGW_32bit-Release/dist/ -lQTwain1

INCLUDEPATH += $$PWD/../qt_twain_wrapper
DEPENDPATH += $$PWD/../qt_twain_wrapper
