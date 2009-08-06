TEMPLATE = app
include(qtsingleapplication/qtsingleapplication.pri)
QT += network \
    xml
TARGET = imageshack
LIBS += -L. \
    -lavformat \
    -lavcodec \
    -lswscale \
    -lavutil# -static
INCLUDEPATH += qtsingleapplication
DEFINES += DEVELOPER_KEY="\\\"$$(IMAGESHACK_DEV_KEY)\\\""
warning($$DEFINES)
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
    limitedplaintextedit.cpp
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
    defines.h
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
    twitterwindow.ui
RESOURCES += images_rc.qrc
TRANSLATIONS += translations/ru_RU.ts \
                translations/en_US.ts
win32:RC_FILE = windowsicon.rc
macx:ICON = macicon.icns
target.path = $$[QT_INSTALL_BINS]
win32:target.path = release
trans.path = /usr/share/imageshack/translations
win32:trans.path = release/translations
trans.files = translations/*qm
INSTALLS += target trans
