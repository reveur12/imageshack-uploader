#include "trayicon.h"

#include <QUrl>
#include <QIcon>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QImage>
#include <QRegExp>
#include <QTemporaryFile>
#include <QDebug>

TrayIcon::TrayIcon(QMainWindow *w)
{
    parent = w;
    icon.setParent(w);
    icon.setIcon(QIcon(":/images/images/windowsicon.ico"));
    QAction *def = menu.addAction(QIcon(":/images/images/logo.png"),
                                 tr("Show/Hide Uploader"),
                                 this, SLOT(showUploader()));
    menu.addAction(QIcon(":/images/images/add.png"),
                   tr("Add file from clipboard"), this, SLOT(addFile()));
    menu.addAction(QIcon(":/images/images/clear.png"), tr("Quit"),
                   this, SLOT(quit()));
    menu.setDefaultAction(def);
    icon.setContextMenu(&menu);
    connect(&icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));
    connect(this, SIGNAL(addFiles(QStringList)),
            parent, SLOT(addFiles(QStringList)));
}

void TrayIcon::show()
{
    icon.show();
}

void TrayIcon::hide()
{
    icon.hide();
}

void TrayIcon::addFile()
{
    QClipboard *c = QApplication::clipboard();
    const QMimeData *mime = c->mimeData();
    QStringList files;
    if (mime->hasImage())
    {
        QImage im = qvariant_cast<QImage>(mime->imageData());
        QTemporaryFile tmp("XXXXXX_image.png");
        tmp.open();
        tmp.setAutoRemove(false);
        im.save(&tmp, "PNG");
        tmp.close();
        files.append(tmp.fileName());
    }
    else if (mime->hasUrls())
    {
        foreach(QUrl url, mime->urls())
        {
            if (!url.toLocalFile().isEmpty())
                files.append(url.toLocalFile());
        }
    }
    else if (mime->hasText())
    {
        QString txt = mime->text();
        files.append(txt.split(QRegExp("\\s", Qt::CaseInsensitive, QRegExp::RegExp2)));
    }
    emit addFiles(files);
}

void TrayIcon::showUploader()
{
    if (!parent->isVisible())
    {
        parent->show();
        parent->raise();
        parent->activateWindow();
    }
    else parent->hide();
}

void TrayIcon::iconClicked(QSystemTrayIcon::ActivationReason reason)
{
    if (reason != QSystemTrayIcon::Trigger) return;
    showUploader();
}

void TrayIcon::quit()
{
    QApplication::quit();
}
