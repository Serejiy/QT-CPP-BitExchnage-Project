# Automatic tests (Qt Test). In Qt Creator: open this file and press Run.
QT += testlib widgets network charts sql
CONFIG += c++17 testcase
TARGET = tst_bitexchange

APP = $$PWD/..
INCLUDEPATH += $$APP

SOURCES += tst_bitexchange.cpp \
    $$APP/database.cpp \
    $$APP/binanceclient.cpp \
    $$APP/candlesticksview.cpp \
    $$APP/mainwindow.cpp

HEADERS += $$APP/database.h \
    $$APP/binanceclient.h \
    $$APP/candlesticksview.h \
    $$APP/mainwindow.h

FORMS += $$APP/mainwindow.ui
