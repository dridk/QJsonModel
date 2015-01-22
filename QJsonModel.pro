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
    jsonitem.cpp \
    qjsonmodel.cpp

HEADERS  += mainwindow.h \
    jsonitem.h \
    qjsonmodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
