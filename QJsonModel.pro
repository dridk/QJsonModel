#-------------------------------------------------
#
# Project created by QtCreator 2015-01-22T08:20:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QJsonModel
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qjsonmodel.cpp \
    qjsonitem.cpp

HEADERS  += mainwindow.h \
    qjsonmodel.h \
    qjsonitem.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
