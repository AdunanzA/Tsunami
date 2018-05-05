<h1 align="center">Tsunami</h1>

<p align="center"><a href="https://www.adunanza.net/" target="_blank"><img alt="Tsunami" src="https://user-images.githubusercontent.com/17850495/30519264-e00d0236-9b91-11e7-8ac9-95393138693c.jpg"></a></p>

<p align="center"><b>An intuitive Torrent client</b></p>

<p align="center">
  <img src="https://ci.appveyor.com/api/projects/status/w8imugyiik0199lx?svg=true" alt="Build Status">
  <img src="https://img.shields.io/github/issues/AdunanzA/Tsunami.svg" alt="Issues">
  <img src="https://img.shields.io/github/forks/AdunanzA/Tsunami.svg" alt="Forks">
  <img src="https://img.shields.io/github/stars/AdunanzA/Tsunami.svg" alt="Stars">
  <img src="https://img.shields.io/badge/license-MPLv2-blue.svg" alt="License">
</p>

Tsunami is a <a href="https://www.qt.io/">Qt</a> application based on <a href="https://github.com/arvidn/libtorrent" target="_blank">libtorrent</a> with innovative features like Streaming, Search and Intelligent Auto-Sharing

We are currently setting up the team volunteers who will develop the software. We accept pull request.

### How to compile:
<h3>Windows:</h3>
- Install Qt, Visual Studio 2015 and git
- Clone the project
- Download the libraries from <a href="http://tsunami.adunanza.net/updates/libraries.7z">here</a>
- Open the archive you previously downloaded and extract the includes and libraries subdirectories in the project folder
- Open Tsunami.pro in qtcreator
- Compile
- Enjoy!

<h3>Ubuntu(16.04):</h3>
- Install Qt, git g++,
- Clone the project;
- sudo add-apt-repository ppa:ntadej/tano # VLC QT libraries
- sudo apt-get install libvlc-qt-core2 libvlc-qt-widgets2 libvlc-qt-dbg libvlc-qt-dev
- sudo apt-get install libttorrent-rasterbar-dev # libtorrent
- Open Tsunami.pro in qtcreator;
- Replace this line "DEFINES += TORRENT_NO_DEPRECATE" with this one "#DEFINES += TORRENT_NO_DEPRECATE" *
- Compile
- Enjoy!

*) We are developing Tsunami without libtorrent's deprecated functions, but the pre-compiled version of ubuntu has this feature enabled by default.
   That's the reason why you must comment the line we mentioned above. In order to compile tsunami without deprecated functions you have to
   compile libtorrent by your own with depreacted functions disabled. 

<hr/>
https://www.adunanza.net/

https://forum.adunanza.net/
