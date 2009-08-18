TEMPLATE = app
include(qtsingleapplication/qtsingleapplication.pri)
QT += network \
    xml
TARGET = ImageShackUploader
unix:!macx:TARGET = imageshack-uploader
unix {
    LIBRARIES = $$system(pkg-config --libs libavcodec libavformat libswscale libavutil)
    isEmpty(LIBRARIES):error(Could not find ffmpeg libraries)
    LIBS += $$LIBRARIES
}
win32:LIBS += -L. \
    -lavformat \
    -lavcodec \
    -lswscale \
    -lavutil
INCLUDEPATH += qtsingleapplication
macx:INCLUDEPATH += $$system(pkg-config --cflags-only-I libavcodec libavformat libswscale libavutil | sed s/-I//g)
UNIX_TRANSLATIONS_DIR = "/usr/share/imageshack-uploader/translations"
DEFINES += UNIX_TRANSLATIONS_DIR="\\\"$$UNIX_TRANSLATIONS_DIR\\\""
VERSION = 2.0.1
DEFINES += VERSION="\\\"$$VERSION\\\""
DEVKEY = $$(IMAGESHACK_DEVELOPER_KEY)
isEmpty(DEVKEY):error(IMAGESHACK_DEVELOPER_KEY variable should be set for building)
DEFINES += DEVELOPER_KEY="\\\"$$DEVKEY\\\""
SOURCES += main.cpp \
    mainwindow.cpp \
    uploadrequest.cpp \
    media.cpp \
    medialistmodel.cpp \
    medialistwidget.cpp \
    loginwidget.cpp \
    progresswidget.cpp \
    mediawidget.cpp \
    clickablelabel.cpp \
    loginrequest.cpp \
    fileuploader.cpp \
    resultswindow.cpp \
    copyablelineedit.cpp \
    clickablelineedit.cpp \
    medialoader.cpp \
    tagwidget.cpp \
    aboutwindow.cpp \
    updatechecker.cpp \
    imageinfowidget.cpp \
    selectabletextedit.cpp \
    talkingtreeview.cpp \
    optionsdialog.cpp \
    copyabletextedit.cpp \
    twitterclient.cpp \
    windowsexplorerintegrator.cpp \
    twitterwindow.cpp \
    videoframereader.cpp \
    gallerycreator.cpp \
    limitedplaintextedit.cpp \
    videopreviewcreator.cpp \
    filesource.cpp \
    ffmpeg_fas.c \
    seek_indices.c \
    advancedcheckbox.cpp
HEADERS += mainwindow.h \
    uploadrequest.h \
    media.h \
    medialistmodel.h \
    medialistwidget.h \
    loginwidget.h \
    progresswidget.h \
    mediawidget.h \
    clickablelabel.h \
    loginrequest.h \
    fileuploader.h \
    resultswindow.h \
    copyablelineedit.h \
    clickablelineedit.h \
    medialoader.h \
    tagwidget.h \
    aboutwindow.h \
    updatechecker.h \
    imageinfowidget.h \
    selectabletextedit.h \
    talkingtreeview.h \
    optionsdialog.h \
    copyabletextedit.h \
    twitterclient.h \
    windowsexplorerintegrator.h \
    twitterwindow.h \
    videoframereader.h \
    gallerycreator.h \
    limitedplaintextedit.h \
    filesource.h \
    videopreviewcreator.h \
    ffmpeg_fas.h \
    seek_indices.h \
    defines.h \
    advancedcheckbox.h
FORMS += mainwindow.ui \
    medialistwidget.ui \
    loginwidget.ui \
    progresswidget.ui \
    mediawidget.ui \
    resultswindow.ui \
    copyablelineedit.ui \
    tagwidget.ui \
    aboutwindow.ui \
    imageinfowidget.ui \
    optionsdialog.ui \
    copyabletextedit.ui \
    twitterwindow.ui \
    advancedcheckbox.ui
RESOURCES += images_rc.qrc
TRANSLATIONS += translations/ru_RU.ts \
    translations/en_US.ts
win32:RC_FILE = windowsicon.rc
macx:ICON = macicon.icns
target.path = $$[QT_INSTALL_BINS]
win32:target.path = release
trans.path = $$UNIX_TRANSLATIONS_DIR
win32:trans.path = release/translations
macx:trans.path = Contents/Resources
trans.files += translations/*qm
win32:trans.commands = lrelease translations\\ru_RU.ts translations\\en_US.ts
unix:trans.commands = lrelease translations/*ts
mactrans.target = mactrans
mactrans.files += translations/en_US.qm \
    translations/ru_RU.qm
mactrans.commands = lrelease \
    translations/*ts
mactrans.path = Contents/Resources
menuitem.target = menuicon
menuitem.files = imageshackuploader.desktop
menuitem.path = /usr/share/applications/
menuicon.target = menuicon
menuicon.files = images/imageshack.png
menuicon.path = /usr/share/pixmaps/
INSTALLS += target \
    trans
!macx:unix:INSTALLS += menuicon \
    menuitem
PRE_TARGETDEPS = trans
macx:PRE_TARGETDEPS = mactrans
QMAKE_BUNDLE_DATA += mactrans
deb.target = deb
deb.commands = rm \
    -rf \
    deb \
    && \
    mkdir \
    -p \
    deb/usr/bin \
    && \
    mkdir \
    -p \
    deb/$$UNIX_TRANSLATIONS_DIR \
    && \
    cp \
    $$TARGET \
    deb/usr/bin/imageshack-uploader \
    && \
    cp \
    translations/ru_RU.qm \
    deb$$UNIX_TRANSLATIONS_DIR/ru_RU.qm \
    && \
    cp \
    translations/en_US.qm \
    deb$$UNIX_TRANSLATIONS_DIR/en_US.qm \
    && \
    mkdir \
    -p \
    deb/usr/share/applications \
    && \
    mkdir \
    -p \
    deb/usr/share/pixmaps \
    && \
    cp \
    images/imageshack.png \
    deb/usr/share/pixmaps/imageshack.png \
    && \
    cp \
    imageshackuploader.desktop \
    deb/usr/share/applications/ \
    && \
    mkdir \
    -p \
    deb/DEBIAN \
    && \
    echo \
    \"Package: \
    imageshack-uploader\" \
    > \
    deb/DEBIAN/control \
    && \
    echo \
    \"Version: \
    $$VERSION\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    \"Section: \
    web\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    \"Priority: \
    optional\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    \"Architecture: \
    all\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    \"Essential: \
    no\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    \"Depends: \
    ffmpeg, \
    libqt4-gui, \
    libqt4-core, \
    libqt4-xml\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    \"Installed-Size: \
    584183\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    \"Maintainer: \
    ImageShack \
    Corp. \
    <support@imageshack.us>\" \
    >> \
    deb/DEBIAN/control \
    && \
    echo \
    "Description: A simple application for uploading one or more images to Imageshack. You may upload to your account or anonymously. Features included tags, previews, image resizing, drag and drop, link creation and more." \
    >> \
    deb/DEBIAN/control \
    && \
    mkdir \
    -p \
    dist \
    && \
    dpkg \
    -b \
    deb \
    dist/imageshack-uploader-$$VERSION\.deb
rpm.target = rpm
rpm.commands = sudo \
    alien \
    --to-rpm \
    dist/imageshack-uploader-$$VERSION\.deb \
    && \
    mv \
    imageshack-uploader-$$VERSION-2.i686.rpm \
    dist/imageshack-uploader-2.0\.rpm
rpm.depends = deb
packages.target = packages
packages.depends = deb \
    rpm
clean.target = clean
clean.commands = rm \
    -f \
    moc_*.cpp \
    && \
    rm \
    -f \
    qrc_*_rc.cpp \
    && \
    rm \
    -f \
    *~ \
    core \
    *.core \
    && \
    rm \
    -f \
    ui_*.h \
    && \
    rm \
    -f \
    *.o \
    && \
    rm \
    -rf \
    $$TARGET \
    && \
    rm \
    -rf \
    dist \
    && \
    rm \
    -rf \
    deb
macx:clean.commands = rm \
    -f \
    moc_*.cpp \
    && \
    rm \
    -f \
    qrc_*_rc.cpp \
    && \
    rm \
    -f \
    *~ \
    core \
    *.core \
    && \
    rm \
    -f \
    ui_*.h \
    && \
    rm \
    -f \
    *.o \
    && \
    rm \
    -rf \
    $$TARGET\.app \
    && \
    rm \
    -rf \
    ImageShackUploader-$$VERSION\.dmg \
    && \
    rm \
    -rf \
    dist \
    && \
    rm \
    -rf \
    deb
QMAKE_EXTRA_TARGETS += deb \
    rpm \
    dmg \
    msi \
    packages \
    clean \
    trans \
    mactrans
dmg.target = dmg
dmg.depends = all
dmg.commands = macdeployqt \
    $$TARGET\.app -dmg && mv $$TARGET\.dmg $$TARGET-$$VERSION\.dmg

msi.target = msi
msi.delends = all
QTDIR = $$replace(QMAKE_LIBDIR_QT, "/", "\\")\..
msi.commands = mkdir dlls | \
               copy $$QTDIR\bin\QtCore4.dll dlls && \
               copy $$QTDIR\bin\QtGui4.dll dlls && \
               copy $$QTDIR\bin\QtXml4.dll dlls && \
               copy $$QTDIR\bin\QtNetwork4.dll dlls && \
               copy $$QTDIR\bin\mingwm10.dll dlls && \
               mkdir dlls\imageformats | \
               copy $$QTDIR\plugins\imageformats\qgif4.dll dlls\imageformats && \
               copy $$QTDIR\plugins\imageformats\qico4.dll dlls\imageformats && \
               copy $$QTDIR\plugins\imageformats\qjpeg4.dll dlls\imageformats && \
               copy $$QTDIR\plugins\imageformats\qmng4.dll dlls\imageformats && \
               copy $$QTDIR\plugins\imageformats\qsvg4.dll dlls\imageformats && \
               copy $$QTDIR\plugins\imageformats\qtiff4.dll dlls\imageformats && \
               copy avcodec.dll dlls && \
               copy avutil.dll dlls && \
               copy avformat.dll dlls && \
               copy swscale.dll dlls && \
               candle ImageShackUploader.wxs && \
               light -ext WixUIExtension ImageShackUploader.wixobj
