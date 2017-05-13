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

# Needed to avoid inclusion error with WinSock2.h (when using libtorrent)
# http://stackoverflow.com/a/8294669
DEFINES += WIN32_LEAN_AND_MEAN

# EXTRA DEFINITIONS
include(version.pri)
include(utility.pri)

SOURCES += main.cpp\
    mainwindow.cpp \
    settingswindow.cpp \
    searchwindow.cpp \
    downloadwindow.cpp \
    tsucard.cpp \
    statisticswindow.cpp

HEADERS  += \
    mainwindow.h \
    settingswindow.h \
    searchwindow.h \
    downloadwindow.h \
    tsucard.h \
    statisticswindow.h

FORMS    += \
    mainwindow.ui \
    settingswindow.ui \
    searchwindow.ui \
    downloadwindow.ui \
    tsucard.ui \
    statisticswindow.ui

OBJECTS_DIR = tmp
MOC_DIR = tmp

QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 5
RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_rel/ -llibtorrent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -llibtorrent
else:unix: LIBS += -L$$PWD/lib/ -llibtorrent
unix: LIBS += -L$$PWD/lib/ -lboost_system-vc140-mt-gd-1_60

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lboost_system-vc140-mt-gd-1_60
else:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_rel/ -lboost_system-vc140-mt-1_60

DISTFILES +=
