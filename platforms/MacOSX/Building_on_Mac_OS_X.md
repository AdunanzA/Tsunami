### PREMISE:
I really don't know anything about *Qt*, my experience with it it's limited to some hours afted dinner, and also my experience with development in Mac OS environment is quite... poor (to be gentle).
So just consider this work just as a "starting point", something to inspire real Qt Mac developer and something that can be improved.
Feel free to contribute :-)

### WHAT I DID:
I used a *Mac Mini Late 2018* running *Mac OS* 10.14.2 *Mojave*. Once ended this little doc, I'll try again the build processing also on a old *Mac Mini Late 2009* running *Mac OS* 10.11.6 *El Capitan*.

I already had installed:
- *XCode* and *XCode command line tools* (**THESE ARE REQUIRED**, please see https://developer.apple.com)
- *MacPorts* (with git and other utilities and development tools)

*Tsunami* is a C++ (C++11) *Qt* project that uses *boost*, *libtorrent* and *VLC-Qt*.
As first I tried to use pre-built binaries from *MacPorts* (it already provides *boost* and *libtorrent-rasterbar*) but, after some difficulies in linking with *libtorrent*, I opted for my own builds of *boost* and *libtorrent*.
Luckily, *VLC-Qt* already provides frameworks that can be used to build *Tsunami*.

So, here my "*(for sure can be improved)* to do list":

### PREMISE 2:
here I simply list some basic instructions I used to build the package. You are *STRONGLY* encouraged to read all docs presents in all downloaded packages.

1. If not already installed, download and install *XCode* and *XCode command line tools* (see https://developer.apple.com and Apple Store). THIS IS MANDATORY!
Before proceeding with following steps make sure to launch it and to accept license.


2. Download and install *Qt*
   Download *Qt* (https://www.qt.io), when downloading, choose the open source version.
   If you choose the default option, you will download the packege of the online installer. Once downloaded, launch it.
   At a certain point, this installer will propose you a list of objects to install, make sure to select at least
   * Qt 5.12.0
     * macOS
     * Qt Script (deprecated)
   * Tools Qt Creator 4.8.0 (or newer)
   
   I also installed *Tools/Qt Installer Framework 3.0* but I'm not so sure that it is strictly needed.
   **WARNING** If you have a Mac OS X version earlier than 10.12 *Sierra* you'll probably need some other version of *Qt* and/or *Qt Creator* (for Mac OS X 10.11 *El Capitan* I used *Qt 5.11.0* and *Creator 4.7.3*, if you need some old versions of Qt I suggest to download and use the **offline** installers)

3. some preparations...

   1. Determine the number of processors/threads available in your machine.
      This is needed to set the **-j** value used by build (just to speedup a little bit the process).
      The simplest way is probably to open a terminal and use the command

        `sysctl -n hw.ncpu`

      On my Mac Mini 2018 equipped with *i7* I obtain the value *12*.
      Remember that will use the result of previous command +1 (so, in my case, *13*).

   2. open a terminal and create a directory to use where to build everything, for example

       `mkdir -p ~/TSUNAMI`

   3. create also a dir where to uncompress various sources

       `mkdir -p ~/TSUNAMI/src`

4. *BOOST*
  download *boost* sources from www.boost.org

    1. uncompress downloaded source archive in source directory created before

        `cd ~/TSUNAMI/src`
        `tar xvf ~/Downloads/boost_1_69_0.tar.bz2`

         (note: if you have no tar command I suggest to use MacPorts version, or you can download archive in ZIP format and uncompress it, but probably, soon or later, you'll need also tar)

    2. create destination directory for *boost* built objects (they will be used to build *Tsunami* later)

       `mkdir -p ~/TSUNAMI/myboost`

    3. go to *boost* sources directory

       `cd ~/TSUNAMI/src/boost_1_69_0`

    4. prepare build process telling boost where to install built objects
       WARNING: do not use *~* in following commands, use instead **${HOME}** or write the real path

       `./bootstrap.sh --prefix=${HOME}/TSUNAMI/myboost`

    5. build *boost* with

       `./b2 -j<num calculated at point 2.a>`

       so in my case I use

       `./b2 -j13`

    6. wait... (coffee :coffee: time?)

    7. install in directory specified at point 4.4 with

         `./b2 install`

    8. wait (yes, again)...

    9. now it's the time to build the *Boost Build* tool (another *b2*) so

       `cd tools/build`

        `./bootstrap.sh`

        `./b2 install --prefix=${HOME}/TSUNAMI/myboost`

    10. Remember to add `~/TSUNAMI/myboost/bin` (I simply edited the *~/.profile* file and restarted the terminal)

5. *LIBTORRENT*
   Download libtorrent sources from https://www.libtorrent.org/ https://github.com/arvidn/libtorrent/releases
   Remember to choose a source code archive!
   I used *libtorrent 1.1.11*

   1. uncompress downloaded archive

      `cd ~/TSUNAMI/src`

      `tar xvf ~/Downloads/libtorrent-libtorrent_1_1_11.tar.gz` (warning: if you used *Safari* to download probaly it already executed *gunzip* and "removed" the trailing *.gz*, in this case just omit the *.gz* suffix)

   2. prepare to build *libtorrent*: set some environment variables

      1. as first set *BOOST_ROOT* with folder name containing *boost* sources

         `export BOOST_ROOT=${HOME}/TSUNAMI/src/boost_1_69_0`

      2. then *BOOST_BUILD_PATH* with folder containing the *boost build* tool sources

         `export BOOST_BUILD_PATH=${HOME}/TSUNAMI/src/boost_1_69_0/tools/build`

      3. **NOW VERY IMPORTANT:** set *CXXFLAGS* to use C++11 (if you forget this step probably you'll have link problems while linking Tsunami, related to "wait_for_alert")

         `export CXXFLAGS=-std=c++11`

   3. build *libtorrent*

      `cd libtorrent-libtorrent_1_1_11`

      `b2 link=static variant=release boost-link=static dht=on encryption=on deprecated-functions=off -j<num calculated at point 2.a>`

      so, in my case, I used

      `b2 link=static variant=release boost-link=static dht=on encryption=on deprecated-functions=off -j13`

   4. wait...

      if everything is ok now you have libtorrent built objects in something like

      *bin/darwin-4.2.1/release/deprecated-functions-off/link-static/threading-multi*


6. *VLC-Qt*
   Download *VLC-Qt* from https://vlc-qt.tano.si/ and uncompress the archive in the src folder created at step 3.3
   In my case, I downloaded the archive *"VLC-Qt_1.1.0_macos.7z"* so now in *~/TSUNAMI/src* I have
```
    mrhyde$ ls -la
    total 0
    drwxr-xr-x   5 mrhyde  staff   160  6 Gen 22:31 .
    drwxr-xr-x   4 mrhyde  staff   128  6 Gen 21:29 ..
    drwxr-xr-x@  6 mrhyde  staff   192  6 Gen 22:30 VLC-Qt_1.1.0_macos
    drwxr-xr-x@ 28 mrhyde  staff   896  6 Gen 22:06 boost_1_69_0
    drwxr-xr-x@ 35 mrhyde  staff  1120  6 Gen 22:20 libtorrent-libtorrent_1_1_11
```
7. Download *Tsunami* sources and put them in folder created at 3.3

   **NOTE** due a little problem with usage of *VLC-Qt* framework we need a little trick to make sure that *Qt Creator* will be able to find all headers of *VLC-Qt* frameworks:

      1. open a terminal and go in ~/TSUNAMI/src/Tsunami folder

         `cd  ~/TSUNAMI/src/Tsunami`

      2. create a folder named **includes** (pay attention at trailing "s"!)

         `mkdir includes`

      3. enter created dir

         `cd includes`

      4. create the following symbolic links to *VLC-Qt* framework header folders:

         `ln -sf ~/TSUNAMI/src/VLC-Qt_1.1.0_macos/lib/VLCQtCore.framework/Headers VLCQtCore`
         `ln -sf ~/TSUNAMI/src/VLC-Qt_1.1.0_macos/lib/VLCQtWidgets.framework/Headers VLCQtWidgets`


8. and not it's *Qt (creator)* time...

   1. launch *Qt Creator* and open Tsunami.pro project

   2. if requested press **Configure project**

   3. click on **Projects** then select **Build** in **Build & Run** section for your kit

      1. choose **Release** for **Edit build configuration**

      2. Select **Run** in **Build & Run** section for your kit

      3. Make sure to check **Add build library search path in *DYLD_LIBRARY_PATH* and *DYLD_FRAMEWORK_PATH***

      4. in **Run Environment**, check **DYLD_FRAMEWORK_PATH**: it must contain the directory where the *VLC-Qt* frameworks are stored, so in my case
      
         `/Users/mrhyde/TSUNAMI/src/VLC-Qt_1.1.0_macos/lib`
         
      5. in **Run Environment**, check **DYLD_LIBRARY_PATH**: it must contain the directory where the *boost* library are stored, so in my case

         `/Users/mrhyde/TSUNAMY/myboost/lib`

      6. just to be sure, on menu bar, select **File->Save All**

      7. now press on **Edit** (vertical menu on the left), expand **Tsunami** tree and then select Tsunami.pro (double click to open it)

      8. look for the **macx** section and customize, according to your environment, the variables

         * FRAMEWORKS_QT_DIR
         * FRAMEWORKS_VLCQT_DIR
         * BOOST_INCLUDES
         * BOOST_LIBS
         * LIBTORRENT_INCLUDES
         * LIBTORRENT_LIBS

      9. from menu, select **Build -> Run qmake**

      10. from menu, select **Build -> Build All**

          If everything is ok now you can launch *Tsunami* (just for test) simply pressing the *play-like* icon in the vertical icon bar (the green triangle on the bottom).
          If *Tsunami* started, close it and prepare the final packet

      11. close *Qt creator*

      12. if you used the default settings, *Qt creator* built everything using a directory named like

          build-Tsunami-*<kit name>*-Release

          and this directory is at the same level of the directory containing the *Tsunami* sources.
          You need to enter that directory

          `cd ~/TSUNAMI/src/build-Tsunami-Desktop_Qt_5_12_0_clang_64bit-Release`

      13. and now let's create the final *Tsunami.app* bundle and the dmg archive using the *Qt* tool *macdeployqt* (it is in *Qt* installation folder) 

          `/Users/mrhyde/Qt/5.12.0/clang_64/bin/macdeployqt Tsunami.app -libpath=${HOME}/TSUNAMI/myboost/lib -dmg`

          Probably some errors will be displayed, anyway if everithing is ok you will find in the same dir the "Tsunami.dmg" file
 
              Mac-di-utonto:build-Tsunami-Desktop_Qt_5_12_0_clang_64bit-Release mrhyde$ ls -latr
              total 126280
              drwxr-xr-x    7 mrhyde  staff       224  6 Gen 23:07 ..
              -rw-r--r--    1 mrhyde  staff      2982  6 Gen 23:07 .qmake.stash
              drwxr-xr-x    3 mrhyde  staff        96  6 Gen 23:07 Tsunami.app
              -rw-r--r--    1 mrhyde  staff   1375807  6 Gen 23:09 Makefile
              -rw-r--r--    1 mrhyde  staff     17559  6 Gen 23:09 ui_searchwindow.h
              -rw-r--r--    1 mrhyde  staff     25026  6 Gen 23:09 ui_settingswindow.h
              -rw-r--r--    1 mrhyde  staff     41506  6 Gen 23:09 ui_mainwindow.h
              -rw-r--r--    1 mrhyde  staff      4594  6 Gen 23:09 ui_downloadwindow.h
              -rw-r--r--    1 mrhyde  staff      1669  6 Gen 23:09 ui_statisticswindow.h
              -rw-r--r--    1 mrhyde  staff     11346  6 Gen 23:09 ui_itemdetails.h
              -rw-r--r--    1 mrhyde  staff      5730  6 Gen 23:09 ui_previewplayerwindow.h
              -rw-r--r--    1 mrhyde  staff      5777  6 Gen 23:09 ui_changelog.h
              -rw-r--r--    1 mrhyde  staff      2875  6 Gen 23:09 ui_archivewindow.h
              -rw-r--r--    1 mrhyde  staff   2478967  6 Gen 23:09 qrc_resources.cpp
              -rw-r--r--    1 mrhyde  staff      9132  6 Gen 23:09 qrc_translations.cpp
              drwxr-xr-x  128 mrhyde  staff      4096  6 Gen 23:09 tmp
              drwxr-xr-x   18 mrhyde  staff       576  6 Gen 23:22 .
              -rw-r--r--@   1 mrhyde  staff  60638380  6 Gen 23:22 Tsunami.dmg <<<==== THAT'S IT!!!

          Move it in some other folder, double click on it and try *Tsunami*! :thumbsup:

