#-------------------------------------------------
#
# Project created by QtCreator 2016-09-24T16:12:49
#
#-------------------------------------------------

unix: LIBS += -lavcodec -lavutil -lavformat -lavdevice -lswscale

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pic_view
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    work.cpp \
    workitem.cpp \
    mylistview.cpp

HEADERS  += mainwindow.h \
    work.h \
    workitem.h \
    mylistview.h

FORMS    += mainwindow.ui
