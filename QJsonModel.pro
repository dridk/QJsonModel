#-------------------------------------------------
#
# Project created by QtCreator 2015-01-22T08:20:52
#
#-------------------------------------------------

QT       += core gui widgets
CONFIG   += c++11
lessThan(QT_MAJOR_VERSION, 5): error("requires Qt 5")

TARGET = QJsonModel
TEMPLATE = app

SOURCES += \
    main.cpp \
    qjsonmodel.cpp

HEADERS += \
    qjsonmodel.h




