QT       += core gui sql network script widgets websockets

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
include(src/version.pri)
include(src/utility.pri)
include(src/tsuCrawler.pri)
include(src/runGuard.pri)
include(src/httpserver.pri)

win32{
 INCLUDEPATH += $$PWD/includes #Do not call "include" this folder!
 DEPENDPATH  += $$PWD/libraries
 DEFINES += WIN32_LEAN_AND_MEAN # http://stackoverflow.com/a/8294669
 DEFINES += _WIN32_WINNT=0x0601
 CONFIG(release, debug|release): LIBS  += -L$$PWD/libraries/rel64/ -L$$PWD/libraries/rel64/vlc_qt/ -lVLCQtCore -lVLCQtWidgets -ltorrent-rasterbar -lboost_system-vc140-mt-1_63
 else:CONFIG(debug, debug|release): LIBS += -L$$PWD/libraries/deb64/ -L$$PWD/libraries/deb64/vlc_qt/ -ltorrent-rasterbar -lboost_system-vc140-mt-gd-1_63 -lVLCQtCored -lVLCQtWidgetsd

 # icon for exe file (for win)
 # here for other os: http://doc.qt.io/qt-4.8/appicon.html
 # Moved here because, outside of this block, it causes problems with macx build
 RC_FILE = resources/ico.qrc
}

# In the following line please notice the ":!macx", it is used to differentiate MAC OS settings from UNIX settings.
# If only unix is specified then a mixture of both is used, with unpredictable (and mostly unwanted) effects!
# (learned hard... this comments it's just a personal remainder)
unix:!macx{
  target.path = .
  INSTALLS += target
  QMAKE_CC = clang
  QMAKE_CXX = clang++
  LIBS += -lVLCQtCore -lVLCQtWidgets -ltorrent-rasterbar -lboost_system
 }

macx{
    # The following is needed to correctlt evaluate @FULL_VERSION@ and @SHORT_VERSION@ in Info.plist
    VERSION=$$PROJECT_VERSION

    # FRAMEWORKS_VLCQT_DIR must be set with the name of the directory where the VLC-Qt frameworks are present.
    #
    # In this directory you need to see something like
    # drwxr-xr-x  6 mrhyde  staff  192 31 Dic 18:29 VLCQtCore.framework
    # drwxr-xr-x  6 mrhyde  staff  192 31 Dic 18:29 VLCQtQml.framework
    # drwxr-xr-x  6 mrhyde  staff  192 31 Dic 18:29 VLCQtWidgets.framework
    # 
    FRAMEWORKS_VLCQT_DIR = /Users/mrhyde/TSUNAMI/src/VLC-Qt_1.1.0_macos/lib

    # Now some variables to find boost objects needed for building process
    # header of boost
    BOOST_INCLUDES = /Users/mrhyde/TSUNAMI/myboost/include
    # library files of boost
    BOOST_LIBS = /Users/mrhyde/TSUNAMI/myboost/lib

    # Now some variables to find libtorrent objects needed for building process
    # header of libtorrent
    LIBTORRENT_INCLUDES = /Users/mrhyde/TSUNAMI/src/libtorrent-libtorrent_1_1_11/include
    # library files of boost
    LIBTORRENT_LIBS = /Users/mrhyde/TSUNAMI/src/libtorrent-libtorrent_1_1_11/bin/darwin-4.2.1/release/deprecated-functions-off/link-static/threading-multi



    # ok, now some stuff to create the bundle app
    # The following lines were "inspired" by:
    # - http://doc.qt.io/qt-5.11/osx-deployment.html
    # - https://www.ics.com/blog/your-guide-deploying-qt-applications-macos
    #
    # Instruct Qt to build the bundle (it is an option enbaled by default for Mac, but just to be sure...)
    CONFIG += app_bundle

    # now icons used by project
    # WARNING: I created the following files converting the big logo Tsunami .ICO file in a PNG (currently it is 256*256)
    # then I resized it to 1024*1024 and used "Image2icon" tool to create the .icns file.
    # Obviously the result can be improved starting from a 1024*1024 image (but I do not have it!) and I guess that this image,
    # in a retina display, will be just orrible.
    #
    ICON = platforms/MacOSX/Tsunami.icns

    QMAKE_INFO_PLIST = platforms/MacOSX/Info.plist

    # let's add some bundle Framework stuff

    # boost system library
    boost_frameworks.path = Contents/Frameworks
    boost_frameworks.files = $${BOOST_LIBS}/libboost_system.dylib
    QMAKE_BUNDLE_DATA += boost_frameworks

    # VLC-QT framewors
    # To install just the required files of VLC-Qt frameworks (= skip Header) I use a silly script
    QMAKE_POST_LINK += sh $${PWD}/platforms/MacOSX/install_vlcqt_frameworks.sh $${FRAMEWORKS_VLCQT_DIR} $${OUT_PWD}

    # Qt frameworks are not needed: they will be installed by macdeployqt tool (see later)


    target.path = .
    INSTALLS += target
    QMAKE_CC = clang
    QMAKE_CXX = clang++

    # Add boost header directory to your include path
    INCLUDEPATH += $${BOOST_INCLUDES}

    # Add libtorrent header directory to your include path
    INCLUDEPATH += $${LIBTORRENT_INCLUDES}

    # Add VLC-Qt framework Headers to include path

    QMAKE_CXXFLAGS += -F$${FRAMEWORKS_VLCQT_DIR}

    # OK, here is my limited knowledge of developing on MacOS and Qt.
    # If I understood well, should be enough to use -F CXXFLAGS (like we do later for link-related flags)
    # but if I do so, I obtain a error like
    #
    # No rule to make target 'VLCQtWidgets/WidgetVideo.h', needed by 'iu_previewplayerwindow.h'
    #
    # my guess: some problems in previewplayerwindow.ui
    #
    # So, at this point, I'll use the famous "tapullo-paradigm"!
    #
    # We want to use the VLC-Qt frameworks, but in the code we have statements like
    #
    #   #include "VLCQtCore/soemthing.h"
    #
    # so the code "expects" to have a VLCQtCore directory.
    # But the frameworks have a directory named "Headers" where the .h file are installed.
    # So here the trick: for the moment just create symbolic links to the "Headers" folder of the framework.
    # I used the same name used in "win32" settings, so I just created, in Tsunami sources, a folder named "includes" and
    # I put the symbolic links here.
    # So my "Tsunami" directory containing the branch appears as listed here:
    # -rw-r--r--   1 mrhyde  staff   3217 31 Dic 18:16 CODE_OF_CONDUCT.md
    # -rw-r--r--   1 mrhyde  staff  16725 31 Dic 18:16 LICENSE
    # -rw-r--r--   1 mrhyde  staff   2621 31 Dic 18:16 README.md
    # -rw-r--r--   1 mrhyde  staff    689 31 Dic 18:16 Tsunami.nuspec
    # -rw-r--r--   1 mrhyde  staff   4182  5 Gen 15:14 Tsunami.pro
    # -rw-rw-rw-   1 mrhyde  staff  24435  5 Gen 14:37 Tsunami.pro.user
    # -rw-r--r--   1 mrhyde  staff   2227 31 Dic 18:16 appveyor.yml
    # drwxr-xr-x   4 mrhyde  staff    128  4 Gen 22:46 includes    <<<<== I created this one!
    # drwxr-xr-x   9 mrhyde  staff    288 31 Dic 18:16 resources
    # -rw-r--r--   1 mrhyde  staff   2641 31 Dic 18:16 snapcraft.yaml
    # drwxr-xr-x  59 mrhyde  staff   1888  5 Gen 15:18 src
    #
    # And in "includes" I have
    #
    # $ cd includes/
    # $ ls -la
    # 
    # drwxr-xr-x   4 mrhyde  staff  128  4 Gen 22:46 .
    # drwxr-xr-x  17 mrhyde  staff  544  5 Gen 15:19 ..
    # lrwxr-xr-x   1 mrhyde  staff   70 31 Dic 20:01 VLCQtCore -> /Users/mrhyde/TSUNAMI/src/VLC-Qt_1.1.0/lib/VLCQtCore.framework/Headers         <<<<== I created this using ln -sf 
    # lrwxr-xr-x   1 mrhyde  staff   73 31 Dic 19:51 VLCQtWidgets -> /Users/mrhyde/TSUNAMI/src/VLC-Qt_1.1.0/lib/VLCQtWidgets.framework/Headers   <<<<== I created this using ln -sf 
    # 
    # Now use the "tapullo" and add our links to include path
    INCLUDEPATH += $${PWD}/includes

    # Add VLC-Qt Framework librarues to linker flags
    QMAKE_LFLAGS += -F$${FRAMEWORKS_VLCQT_DIR} -framework VLCQtCore -framework VLCQtWidgets

    # not really sure about the following line...
    QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../,-rpath,@executable_path/../

    # Add boost libraries to linker flags
    LIBS += -L$${BOOST_LIBS} -lboost_system

    # Add libtorrent libraries to linker flags
    LIBS += -L$${LIBTORRENT_LIBS} -ltorrent

    # NOTE: to test it remember to set your build settings for DYLD_FRAMEWORK_PATH and DYLD_LIBRARY_PATH
    # For example:
    # - click on "Projects"
    # - choose your target kit configuration (in my case "Desktop Qt 5.12.0 clang 64bit)
    # - select "Run"
    # - in "Run Settings" check the option "Add build library search path to DYLD_LIBRARY_PATH and DYLD_FRAMEWORK_PATH"
    # - in "Run Environment" expand Details and add (if missing)
    #   - the folder where the VLC-Qt frameworks are installed (same value of var VLCQTFRAMEWORKSDIR) in var  DYLD_FRAMEWORK_PATH
    #   - directories of other libraries used (boost, libtorrent) in var DYLD_LIBRARY_PATH

    # Another note/reminder to create the package:
    # once build process is ended, open a terminal, go in folder containing built objects and use Qt tool "macdeployqt" to create the dmg pckage.
    # In my environment I use:
    #
    # /Users/mrhyde/Qt/5.12.0/clang_64/bin/macdeployqt Tsunami.app -libpath=${HOME}/TSUNAMI/myboost/lib -dmg -always-overwrite
    #
    # WARNING: remember also that the Info.plist is not recreated in Tsunami.app bundle, so probably it is always better to delete the Tsunami.app bundle from $$OUT_PWD before building the project
    # (or at least delete the Info.plist file from $$OUT_PWD/Tsunami.app/Contents)
    #
}


SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/settingswindow.cpp \
    src/searchwindow.cpp \
    src/downloadwindow.cpp \
    src/statisticswindow.cpp \
    src/itemdetails.cpp \
    src/previewplayerwindow.cpp \
    src/changelog.cpp \
    src/archivewindow.cpp

HEADERS  += \
    src/mainwindow.h \
    src/settingswindow.h \
    src/searchwindow.h \
    src/downloadwindow.h \
    src/statisticswindow.h \
    src/itemdetails.h \
    src/previewplayerwindow.h \
    src/changelog.h \
    src/archivewindow.h

FORMS    += \
    src/mainwindow.ui \
    src/settingswindow.ui \
    src/searchwindow.ui \
    src/downloadwindow.ui \
    src/statisticswindow.ui \
    src/itemdetails.ui \
    src/previewplayerwindow.ui \
    src/changelog.ui \
    src/archivewindow.ui

OBJECTS_DIR = tmp
MOC_DIR = tmp

QMAKE_RESOURCE_FLAGS += -compress 9 -threshold 5

# do not use backslash for paths here: this must work on all platforms (it seems that Mac OS does not like backslashes here)
RESOURCES += \
    resources/resources.qrc \
    resources/translations.qrc

