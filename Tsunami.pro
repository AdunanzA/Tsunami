#-------------------------------------------------
#
# Project created by QtCreator 2017-02-26T09:40:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tsunami
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# VERSION DEFINES
include(version.pri)

SOURCES += main.cpp\
    mainwindow.cpp \
    settingswindow.cpp \
    searchwindow.cpp \
    downloadwindow.cpp

HEADERS  += \
    mainwindow.h \
    settingswindow.h \
    searchwindow.h \
    downloadwindow.h

FORMS    += \
    mainwindow.ui \
    settingswindow.ui \
    searchwindow.ui \
    downloadwindow.ui

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/release/ -lboost_system-vc140-gd-1_60
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug/ -lboost_system-vc140-gd-1_60
#else:unix: LIBS += -L$$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/ -lboost_system-vc140-gd-1_60

#INCLUDEPATH += $$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug
#DEPENDPATH += $$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug

QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 5
RESOURCES += \
    resources.qrc


