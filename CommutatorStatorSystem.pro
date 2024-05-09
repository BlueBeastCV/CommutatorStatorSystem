#-------------------------------------------------
#
# Project created by QtCreator 2023-12-27T17:55:21
#
#-------------------------------------------------

QT       += core gui sql xlsx charts serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CommutatorStatorSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
 #       speedsetting.cpp \
        widget.cpp \
        dataoper.cpp \
        frmmessagebox.cpp \
        iconhelper.cpp \
        Snap7_sdk/plc_siemens.cpp \
        Snap7_sdk/s7.cpp \
        Snap7_sdk/snap7.cpp \
        loginform.cpp \
        savelog.cpp \
        titlewidget.cpp \
        qtstreambuf.cpp \
        BaseGraphicsview.cpp \
        ImageWidget.cpp \
        limitform.cpp \
        mydashboard.cpp



HEADERS += \
#        speedsetting.h \
        widget.h \
        iconhelper.h \
        loginform.h \
        myhelper.h \
        objectinfo.h \
        savelog.h \
        Snap7_sdk/plc_siemens.h \
        Snap7_sdk/s7.h \
        Snap7_sdk/snap7.h \
        stdc++.h \
        titlewidget.h \
        frmmessagebox.h \
        dataoper.h \
        connection.h \
        stdc++.h \
        qtstreambuf.h \
        BaseGraphicsview.h \
        ImageWidget.h \
        limitform.h \
        mydashboard.h



FORMS += \
#        speedsetting.ui \
        widget.ui \
        frmmessagebox.ui \
        limitform.ui \
        loginform.ui



RC_ICONS=log.ico


RESOURCES += \
    images.qrc

include(Snap7_sdk.pri)

LIBS +=  -L$$PWD/USB_SDK/lib/ -llibusb
INCLUDEPATH +=  $$PWD/USB_SDK/include
DEPENDPATH +=  $$PWD/USB_SDK/include

DISTFILES +=
