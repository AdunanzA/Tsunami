# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

Types of changes
Added - for new features.
Changed - for changes in existing functionality.
Deprecated - for soon-to-be removed features.
Removed - for now removed features.
Fixed - for any bug fixes.
Security - in case of vulnerabilities.
[YANKED] - versions that had to be pulled because of a serious bug or security issue

## [Unreleased]

## [0.7.2] 2018-10-07
### Added
- Web Interface

### Fixed
- Error on item date in search page
- Settings page

## [0.7.1] 2018-09-20
### Fixed
- source code refactoring

## [0.7.0] 2018-09-19
### Added
- Search page: added categories selection
- Auto save fastresume data every 10 minutes

### Fixed
- Led on external IP, DHT ecc... now works again
- Missing torrents on restart
- Negative values in tsuCard remaining bytes
- Crash on fastresume file without torrent file

## [0.6.4] 2018-09-18
### Changed
- Search page: search starts when enter is pressed

### Fixed
- Tsunami now respect download folder on fastresume

### Added
- Tsunami close confirmation
- Tsunami now minimize to tray icon (task bar icon is now hidden)

## [0.6.3] 2018-05-26
### Fixed
- Fixed various memory leaks

## [0.6.1] 2018-02-11
### Changed
- libraries and headers structures have been ordered

## [0.6.0] 2018-02-07
### Changed
- libtorrent-raster, boost and ssl

## [0.5.6] 2017-12-05
### Added
- Added support for full screen in Streaming window (double-click on video and dedicated button)
- Reviewed Streming interface
- Added autopause and quick rewind when changing window during Streaming

## [0.5.5] 2017-11-23
### Added
- Added Installer Icon

### Fixed
- Fixed Program Icon


## [0.5.4] 2017-11-19
### Added
- Single instance check
- Filename on tsuCard name tooltip

### Fixed
- Scripts download after update

### Changed
- Patreon button from config page to main page

## [0.5.3] 2017-11-15
### Fixed
- Missing reference

## [0.5.2] 2017-11-14
### Added
- Search engine from torrent site
- Add magnet from clipboard
- Patreon link

### Fixed
- Optimized context menu creation in downloadwindow
- Wrong languange on restart

### Changed
- Window management

## [0.5.1] 2017-11-12
### Fixed
- Appveyor cache

### Added
- Installer icon

## [0.5.0] 2017-10-03
### Added
- Streaming of content directly inside the app using VLC-Qt library

## [0.4.7] 2017-10-01
### Added
- Drag & Drop torrent to add
- Top badges with torrent count and filter
- Message on download finished (and option to disable)
- Animation on tsuCard reorder
- Archive window (still not working)
- Seeds and peers information on tsuCard
- Download splash screen
- Tree widget for archive page

### Fixed
- Appveyor script error for delta nupkg
- Error on multiple add of same torrent
- Wrong sum of send and received bytes

## [0.4.6] 2017-09-24
### Added
- More thread (based on cpu) for libtorrent

### Changed
- Gauge max value on max download rate
- Session startup to catch all events

### Fixed
- Added tsuCard get correct size
- Download and Upload stats on status bar
- Restart problem if updated

## [0.4.5] 2017-09-22
### Added
- Show changelog in settings window
- Timer on tsumanager to post statistic updates

### Changed
- Included CHANGELOG in resources
- Session statistic message queue
- Dynamic max value on gauge

### Fixed
- Wrong session statistic values
- Chart wrong color

### Removed
- Unused log class
- Removed Qscale

## [0.4.4] 2017-09-21
### Changed
- appveyor script to ignore bak file

### Removed
- Unused bak file

## [0.4.3] 2017-09-19
### Added
- updatemanager
- zoom on tsuitem
- check libtorrent connection

### Changed
- new gauge

## [0.4.2] 2017-09-19
### Fixed
- appveyor script
- README

## [0.4.1] 2017-09-15
### Added
- this CHANGELOG

## [0.4.0] 2017-09-10
### Added
- new tsuGraphicsScene
- context menu in tsulist (right mouse)
- tsuCard selection (single item and mouse drag)
- many debug log
- settings to limit download and upload rate
- session state
- restart on language change

### Changed
- optimized itemdetails
- optimized log
- optimized settings window

### Fixed
- tooltip errors on showing
- settings and languages problems
- color problem on graphs

### Removed
- unused images

## [0.3.11] 2017-09-09
### Fixed
- update.exe error (short path name)

### Changed
- appveyor script

## [0.3.10] 2017-09-08
### Changed
- command test

## [0.3.9] 2017-09-05
### Changed
- Updated appveyor to manage only 3 version digit

## [0.3.8] 2017-09-04
### Fixed
- title bar translate error

## [0.3.7] 2017-09-02
### Added
- translation

### Fixed
- auto update bug

### Changed
- appveyor script

## [0.3.6] 2017-09-01
### Added
- included appveyor for auto update on commit

## [0.3.5] 2017-07-03
### Added
- item details to see file and parts
- new thread for sessionManager
- debug level to settings (bugged...)
- optimized settings for libtorrent
- redirected log from console to log file

### Changed
- optimized update and add events
- managed closing sequence and fastresume

## [0.3.4] 2017-07-01
### Fixed
- appveyor script test

## [0.3.3] 2017-06-30
### Fixed
- appveyor version enumerating error

## [0.3.2] 2017-06-17
### Added
- session statistic timer on download page
- pause and resume functions
- events class
- settings in ini file
- button chat link

### Fixed
- optimized events emits
- managed download path on settingspage
- managed state and optimized remaining time in tsuItem
- managed save resume session in tsuManager

## [0.3.1] 2017-06-16
### Fixed
- appveyor script test

## [0.3.0] [YANKED]

## [0.2.0] 2017-06-06
### Changed
- changed from qwidget to qgraphicsitem for tsuItem
- changed libtorrent management to shared::pointer
- moved libtorrent from downloadpage to tsumanager
- updated Ui and ref to libtorrent

## [0.1.1] 2017-05-13
### Added
- downloadwindow with libtorrent session
- chartXY
- tsuProgress (inherited from QSegmentWidget)
- splash screen

## [0.1.0] 2017-04-20
### Added
- tsucard

### Changed
- main layout

### Fixed
- compile in debug and release with libtorrent included

## [0.0.2] 2017-03-19
### Added
- mainwindow 
- basic menu button with content change on click

### Changed
- resizable window: fixed position for add and quit button

## [0.0.1] 2017-03-19
### Added
- qt project
- imported libtorrent library
