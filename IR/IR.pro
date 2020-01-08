#-------------------------------------------------
#
# Project created by QtCreator 2017-05-23T21:00:12
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    highlighter.cpp \
    btree.cpp \
    dialog.cpp \
    funcion_thread.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    btree.h \
    dialog.h \
    funcion_thread.h

FORMS    += mainwindow.ui \
    dialog.ui

RC_ICONS = img/ic_browser.ico

RESOURCES += \
    res.qrc
