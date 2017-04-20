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

# EXTRA DEFINITIONS
include(version.pri)
include(utility.pri)

SOURCES += main.cpp\
    mainwindow.cpp \
    settingswindow.cpp \
    searchwindow.cpp \
    downloadwindow.cpp \
    tsucard.cpp \

HEADERS  += \
    mainwindow.h \
    settingswindow.h \
    searchwindow.h \
    downloadwindow.h \
    tsucard.h \

FORMS    += \
    mainwindow.ui \
    settingswindow.ui \
    searchwindow.ui \
    downloadwindow.ui \
    tsucard.ui

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/release/ -lboost_system-vc140-gd-1_60
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug/ -lboost_system-vc140-gd-1_60
#else:unix: LIBS += -L$$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/ -lboost_system-vc140-gd-1_60

#INCLUDEPATH += $$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug
#DEPENDPATH += $$PWD/../../DHT/boost_1_60_0/bin.v2/libs/system/build/msvc-14.0/debug

#INCLUDEPATH += "$$PWD/lib"
#LIBS += "$$PWD/lib/libtorrent.dll"

QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 5
RESOURCES += \
    resources.qrc

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_rel/ -llibtorrent
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -llibtorrent
#else:unix: LIBS += -L$$PWD/lib/ -llibtorrent
#unix|win32: LIBS += -L$$PWD/lib/ -lboost_system-vc140-mt-gd-1_60

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

#win32:CONFIG(debug, debug|release): DISTFILES += \
#    lib/boost_atomic-vc140-mt-gd-1_60.lib \
#    lib/boost_chrono-vc140-mt-gd-1_60.lib \
#    lib/boost_date_time-vc140-mt-gd-1_60.lib \
#    lib/boost_system-vc140-mt-gd-1_60.lib \
#    lib/boost_thread-vc140-mt-gd-1_60.lib \
#    lib/libtorrent.lib \
#    lib/boost_random-vc140-mt-gd-1_60.dll \
#    lib/boost_system-vc140-mt-gd-1_60.dll \
#    lib/libtorrent.dll \
#else:win32:CONFIG(release, debug|release): DISTFILES += \
#    lib_rel/boost_atomic-vc140-mt-1_60.lib \
#    lib_rel/boost_chrono-vc140-mt-1_60.lib \
#    lib_rel/boost_date_time-vc140-mt-1_60.lib \
#    lib_rel/boost_system-vc140-mt-1_60.dll \
#    lib_rel/boost_system-vc140-mt-1_60.lib \
#    lib_rel/boost_thread-vc140-mt-1_60.lib \
#    lib_rel/libtorrent.dll \
#    lib_rel/libtorrent.lib \
win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -llibtorrent
else:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_rel/ -llibtorrent

win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lboost_system-vc140-mt-gd-1_60
else:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_rel/ -lboost_system-vc140-mt-1_60
