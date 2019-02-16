#!/bin/sh

# $1 - directory containing the VLC-Qt .frameworks
# $2 - directtory containing the Tsunami.app boundle ($${OUT_PWD})

THE_SRC=$1
THE_OUT=$2

THE_FRAMEWORKS=${THE_OUT}/Tsunami.app/Contents/Frameworks
mkdir -p ${THE_FRAMEWORKS}

echo Copying VLCQtCore.framework in ${THE_FRAMEWORKS}...
( cd ${THE_SRC} ; rsync -rv --exclude=.DS_Store --exclude=Headers VLCQtCore.framework ${THE_FRAMEWORKS} ) > /dev/null

echo Copying VLCQtWidgets.framework in ${THE_FRAMEWORKS}...
( cd ${THE_SRC} ; rsync -rv --exclude=.DS_Store --exclude=Headers VLCQtWidgets.framework ${THE_FRAMEWORKS} ) > /dev/null

