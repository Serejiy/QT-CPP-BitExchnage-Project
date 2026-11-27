QT       += core gui network charts sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    binanceclient.cpp \
    candlesticksview.cpp \
    database.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    registration.cpp \
    welcomepage.cpp

HEADERS += \
    binanceclient.h \
    candlesticksview.h \
    database.h \
    login.h \
    mainwindow.h \
    registration.h \
    welcomepage.h

FORMS += \
    login.ui \
    mainwindow.ui \
    registration.ui \
    welcomepage.ui

# Windows: name of the .exe, program icon and version info (right click > Properties > Details)
win32 {
    TARGET = BitExchange
    VERSION = 1.0.0
    RC_ICONS = bitexchange.ico
    QMAKE_TARGET_PRODUCT = BitExchange
    QMAKE_TARGET_DESCRIPTION = BitExchange
    QMAKE_TARGET_COPYRIGHT = "Serhii Ponomarenko"
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
