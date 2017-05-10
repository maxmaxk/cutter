#-------------------------------------------------
#
# Project created by QtCreator 2017-03-22T07:32:26
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cutter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mygraphicsview.cpp \
    logger.cpp

HEADERS  += mainwindow.h \
    mygraphicsview.h \
    logger.h

FORMS    += mainwindow.ui \
    logger.ui

RESOURCES += \
    resurs.qrc
