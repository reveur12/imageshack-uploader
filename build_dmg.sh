#!/bin/sh

#FRAMEWORKS=/opt/local/libexec/qt4-mac/lib/QtXml.framework/Versions/4/QtXml /opt/local/libexec/qt4-mac/lib/QtGui.framework/Versions/4/QtGui /opt/local/libexec/qt4-mac/lib/QtNetwork.framework/Versions/4/QtNetwork /opt/local/libexec/qt4-mac/lib/QtCore.framework/Versions/4/QtCore

hdiutil create -fs HFS+ -srcfolder ImageShackUploader.app  -volname ImageShackUploader $1