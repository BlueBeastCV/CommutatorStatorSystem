#-------------------------------------------------
#
# Project created by QtCreator 2023-12-27T17:55:21
#
#-------------------------------------------------

QT       += core gui sql charts serialport printsupport

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
	axialpolarchart.cpp \
    chartview.cpp \
		main.cpp \
 #       speedsetting.cpp \
 #   mychartview.cpp \
		mwlmanager.cpp \
		radialpolarchart.cpp \
		widget.cpp \
		dataoper.cpp \
		frmmessagebox.cpp \
		iconhelper.cpp \
		Snap7_sdk/plc_siemens.cpp \
		Snap7_sdk/s7.cpp \
		Snap7_sdk/snap7.cpp \
		Snap7_sdk/plcthread.cpp \
		loginform.cpp \
		savelog.cpp \
		titlewidget.cpp \
#		qtstreambuf.cpp \
#		BaseGraphicsview.cpp \
#		ImageWidget.cpp \
		limitform.cpp \
		mydashboard.cpp \
		QChartMouseEvent.cpp



HEADERS += \
#        speedsetting.h \
		axialpolarchart.h \
 #       mychartview.h \
    chartview.h \
		mwlmanager.h \
		radialpolarchart.h \
		widget.h \
		iconhelper.h \
		loginform.h \
		myhelper.h \
		objectinfo.h \
		savelog.h \
		Snap7_sdk/plc_siemens.h \
		Snap7_sdk/s7.h \
		Snap7_sdk/snap7.h \
		Snap7_sdk/plcthread.h \
		titlewidget.h \
		frmmessagebox.h \
		dataoper.h \
		connection.h \
		stdc++.h \
#		qtstreambuf.h \
#		BaseGraphicsview.h \
#		ImageWidget.h \
		limitform.h \
		mydashboard.h \
		QChartMouseEvent.h




FORMS += \
#        speedsetting.ui \
		axialpolarchart.ui \
		radialpolarchart.ui \
		widget.ui \
		frmmessagebox.ui \
		limitform.ui \
		loginform.ui


RC_ICONS=AECC.ico
#adding qtrpt subproject
#INCLUDEPATH += $$PWD/QtRptProject  QtRptProject
#include($$PWD/QtRptProject/QtRPT/QtRPT.pri)

RESOURCES += \
	images.qrc

# 使用qtxlsx源代码   不用编译，编译比较麻烦，特别是更换环境的时候
include(qtxlsx/src/xlsx/qtxlsx.pri)

include(Snap7_sdk.pri)
include(Mwl_SDK.pri)
win32{
contains(QT_ARCH, x86_64){
	#64位
	message("64-bit")
	#Module1
#    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build_64/xxx/xxx/release/ -lxxx
#    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build_64/xxx/xxx/debug/ -lxxx

#    #Module2
#    LIBS += -L$$PWD/../xxx/xxx/x64/ -lxxx
#    INCLUDEPATH += $$PWD/../xxx/xxx/

}else{
	#32位
	message("32-bit")
	#Module1
#    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build/xxx/xxx/release/ -lxxx
#    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build/xxx/xxx/debug/ -lxxx

#    #Module2
#    LIBS += -L$$PWD/../xxx/xxx/x86/ -lxxx
#    INCLUDEPATH += $$PWD/../xxx/xxx/
}
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
