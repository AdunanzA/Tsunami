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

DEFINES += TORRENT_NO_DEPRECATE
DEFINES += _WIN32_WINNT=0x0600
DEFINES += WIN64
DEFINES += BOOST_ALL_DYN_LINK
DEFINES += UNICODE

#DEFINES += _WINDOWS
#DEFINES += _DEBUG
#DEFINES += TORRENT_EXPORT_EXTRA
#DEFINES += TORRENT_BUILDING_LIBRARY
#DEFINES += TORRENT_USE_OPENSSL
#DEFINES += TORRENT_BUILDING_SHARED
#DEFINES += _WIN32_WINNT=0x0600
#DEFINES += BOOST_ALL_NO_LIB
#DEFINES += UNICODE
#DEFINES += _UNICODE
#DEFINES += _SCL_SECURE_NO_DEPRECATE
#DEFINES += _CRT_SECURE_NO_DEPRECATE
#DEFINES += _FILE_OFFSET_BITS=64
#DEFINES += BOOST_EXCEPTION_DISABLE
#DEFINES += BOOST_ASIO_ENABLE_CANCELIO
#DEFINES += CMAKE_INTDIR="Debug"
#DEFINES += torrent_rasterbar_EXPORTS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
    statisticswindow.cpp \
    itemdetails.cpp

HEADERS  += \
    mainwindow.h \
    settingswindow.h \
    searchwindow.h \
    downloadwindow.h \
    statisticswindow.h \
    itemdetails.h

FORMS    += \
    mainwindow.ui \
    settingswindow.ui \
    searchwindow.ui \
    downloadwindow.ui \
    statisticswindow.ui \
    itemdetails.ui

OBJECTS_DIR = tmp
MOC_DIR = tmp

# icon for exe file (for win)
# here for other os: http://doc.qt.io/qt-4.8/appicon.html
RC_FILE = ico.qrc

QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 5
RESOURCES += \
    resources.qrc \
    translations.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_rel/ -llibtorrent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -llibtorrent
else:unix: LIBS += -L$$PWD/lib/ -llibtorrent

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lboost_system-vc140-mt-gd-1_64
else:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_rel/ -lboost_system-vc140-mt-1_64
unix: LIBS += -L$$PWD/lib/ -lboost_system-vc140-mt-gd-1_64

#DISTFILES +=
