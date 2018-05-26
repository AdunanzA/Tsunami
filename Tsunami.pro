QT       += core gui sql network script widgets

TARGET = Tsunami
TEMPLATE = app
CONFIG += c++11

# Used by QSettings
DEFINES += APP_ORGANIZATION_NAME=\"\\\"AdunanzA\\\"\" \
           APP_ORGANIZATION_DOMAIN=\"\\\"adunanza.net\\\"\" \
           APP_PROJECT_NAME=\"\\\"Tsunami\\\"\"

# Qt Defines
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += QT_DEPRECATED_WARNINGS
# Libtorrent Defines
DEFINES += TORRENT_NO_DEPRECATE

# EXTRA DEFINITIONS
include(version.pri)
include(utility.pri)
include(tsuCrawler.pri)
include(runGuard.pri)

win32{
 INCLUDEPATH += $$PWD/includes #Do not call "include" this folder!
 DEPENDPATH  += $$PWD/libraries
 DEFINES += WIN32_LEAN_AND_MEAN # http://stackoverflow.com/a/8294669
 DEFINES += _WIN32_WINNT=0x0601
 CONFIG(release, debug|release): LIBS  += -L$$PWD/libraries/rel64/ -L$$PWD/libraries/rel64/vlc_qt/ -lVLCQtCore -lVLCQtWidgets -ltorrent-rasterbar -lboost_system-vc140-mt-1_63
 else:CONFIG(debug, debug|release): LIBS += -L$$PWD/libraries/deb64/ -L$$PWD/libraries/deb64/vlc_qt/ -ltorrent-rasterbar -lboost_system-vc140-mt-gd-1_63 -lVLCQtCored -lVLCQtWidgetsd
}

unix:!macx{
 target.path = .
 INSTALLS += target
 QMAKE_CC = clang
 QMAKE_CXX = clang++
 LIBS += -lVLCQtCore -lVLCQtWidgets -ltorrent-rasterbar -lboost_system
}

SOURCES += main.cpp\
    mainwindow.cpp \
    settingswindow.cpp \
    searchwindow.cpp \
    downloadwindow.cpp \
    statisticswindow.cpp \
    itemdetails.cpp \
    previewplayerwindow.cpp \
    changelog.cpp \
    archivewindow.cpp

HEADERS  += \
    mainwindow.h \
    settingswindow.h \
    searchwindow.h \
    downloadwindow.h \
    statisticswindow.h \
    itemdetails.h \
    previewplayerwindow.h \
    changelog.h \
    archivewindow.h

FORMS    += \
    mainwindow.ui \
    settingswindow.ui \
    searchwindow.ui \
    downloadwindow.ui \
    statisticswindow.ui \
    itemdetails.ui \
    previewplayerwindow.ui \
    changelog.ui \
    archivewindow.ui

OBJECTS_DIR = tmp
MOC_DIR = tmp

# icon for exe file (for win)
# here for other os: http://doc.qt.io/qt-4.8/appicon.html
RC_FILE = ico.qrc

QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 5
RESOURCES += \
    resources.qrc \
    translations.qrc
