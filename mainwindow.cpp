/*
Copyright (c) 2009, ImageShack Corp.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the ImageShack nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QMessageBox>
#include <fileuploader.h>
#include "mediawidget.h"
#include <resultswindow.h>
#include "defines.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tr("LANGUAGE_NAME"); //for translation file
    setWindowTitle(tr("ImageShack Uploader %1").arg(QApplication::applicationVersion()));
    ui->tagWidget->setMediaList(ui->mediaList->getModel());
    connect(ui->mediaList,
            SIGNAL(mediaSelected(QSharedPointer<Media>)),
            this,
            SLOT(mediaSelected(QSharedPointer<Media>)));
    /*connect(ui->mediaList,
            SIGNAL(mediaSelected(QSharedPointer<Media>)),
            ui->tagWidget,
            SLOT(setMedia(QSharedPointer<Media>)));*/
    connect(ui->mediaList,
            SIGNAL(modelReset()),
            this,
            SLOT(mediasReseted()));
    /*connect(ui->mediaList,
            SIGNAL(modelReset()),
            ui->tagWidget,
            SLOT(unsetMedia()));*/
    connect(ui->mediaList,
            SIGNAL(mediasChanged()),
            this,
            SLOT(mediasChanged()));
    connect(ui->mediaList,
            SIGNAL(requireProgressbar(bool)),
            this,
            SLOT(mediaListProgressbarReceiver(bool)));
    connect(ui->mediaList,
            SIGNAL(progress(int,int)),
            this,
            SLOT(mediaListProgressReceiver(int,int)));
    connect(ui->loginWidget,
            SIGNAL(loginStatus(int)),
            ui->tagWidget,
            SLOT(loginStatusReceiver(int)));
    connect(ui->loginWidget, SIGNAL(loginStatus(int)),
            ui->imageInfo, SLOT(loginStatusReceiver(int)));
    connect(ui->tagWidget,
            SIGNAL(batchPrivacy(bool)),
            ui->imageInfo,
            SLOT(batchPrivacy(bool)));
    controlsDisabled = false;

    QSettings sets;
    if (sets.value("autoupdate", QVariant(true)).toBool())
        checkUpdatesSilently();

    about = new AboutWindow(this);
    options = new OptionsDialog(this);
    reswindow = new ResultsWindow(this);

    icon = QSharedPointer<TrayIcon>(new TrayIcon(this));
    icon.data()->show();
}



void MainWindow::upload()
{
    if (!ui->mediaList->getModel()->rowCount(QModelIndex())) return;
    if (!ui->loginWidget->isLoggedIn())
    foreach(QSharedPointer<Media> media, ui->mediaList->getAllMedias())
    {
        if (media.data()->getType() == "x-shockwave-flash")
        {
            int ans = QMessageBox::warning(this, tr("Warning"),
                                 tr("Flash files can not be uploaded without logging in. Do you want to try anyway?"),
                                 QMessageBox::Yes, QMessageBox::No);
            if (ans == QMessageBox::No) return;
            break;
        }
    }
    uploader = QSharedPointer<FileUploader>(new FileUploader(ui->progressWidget, ui->mediaList->getModel()));
    if (ui->loginWidget->isLoggedIn())
        uploader.data()->setCookie(ui->loginWidget->getCookie());
    connect(uploader.data(),
            SIGNAL(results(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > >)),
            this,
            SLOT(uploadResultReceiver(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > >)));
    connect(uploader.data(), SIGNAL(status(int)),
            this, SLOT(uploadStatusReceiver(int)));
    connect(ui->progressWidget, SIGNAL(cancel()),
            uploader.data(), SLOT(cancel()));
    connect(ui->progressWidget, SIGNAL(pause(bool)),
            uploader.data(), SLOT(pause(bool)));
    connect(uploader.data(), SIGNAL(ETA(int)),
            ui->progressWidget, SLOT(updateETA(int)));
    uploader.data()->begin();
}

void MainWindow::disableControls()
{
    ui->loginWidget->setEnabled(false);
    //ui->tagWidget->setEnabled(false);
    ui->tagWidget->disable();
    ui->imageInfo->disable();
    ui->mediaList->setEnabled(false);             // DO NOT TOUSH THIS FUNCTION
    controlsDisabled = true;
    setCursor(Qt::WaitCursor);
    ui->loginWidget->setCursor(Qt::WaitCursor);
    ui->mediaList->setCursor(Qt::WaitCursor);
}

void MainWindow::enableControls()
{
    qDebug() << "enable controls";
    ui->loginWidget->setEnabled(true);
        //if (ui->loginWidget->isLoggedIn()) ui->tagWidget->setEnabled(true);
        //ui->imageInfo->enable();
    ui->tagWidget->enable();
    ui->mediaList->setEnabled(true);
    controlsDisabled = false;                    // DO NOT TOUSH THIS FUNCTION
    setCursor(Qt::ArrowCursor);
    ui->loginWidget->setCursor(Qt::ArrowCursor);
    ui->mediaList->setCursor(Qt::ArrowCursor);

    if (ui->mediaList->getModel()->rowCount())
        ui->mediaList->selectFirst();
}

void MainWindow::mediaSelected(QSharedPointer<Media> media)
{
    if (controlsDisabled) return;
    ui->imageInfo->setMedia(media);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mediasReseted()
{
    ui->imageInfo->reset();
}

void MainWindow::uploadResultReceiver(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > > results)
{
    reswindow->setResults(results);
    reswindow->show();
}

void MainWindow::mediaListProgressbarReceiver(bool value)
{
    if (value) ui->progressWidget->setCurrentIndex(2);
    else ui->progressWidget->setCurrentIndex(0);
}

void MainWindow::mediaListProgressReceiver(int current, int total)
{
    ui->progressWidget->setMediaLoadProgress(current, total);
}

void MainWindow::uploadStatusReceiver(int state)
{
    if (state == 0) disableControls();
    else enableControls();
}

void MainWindow::showAbout()
{
    about->show();
}

void MainWindow::openSupport()
{
    QUrl url(SUPPORT_URL);
    QDesktopServices::openUrl(url);
}

void MainWindow::checkUpdates()
{
    updateChecker.check();
}

void MainWindow::checkUpdatesSilently()
{
    updateChecker.check(true);
}

void MainWindow::mediasChanged()
{
    ui->progressWidget->setUploadEnabled(ui->mediaList->getModel()->rowCount());
    if (!ui->mediaList->getModel()->rowCount())
    {
        ui->imageInfo->disable();
        ui->tagWidget->setEnabled(false);
    }
    else
    {
        ui->tagWidget->setEnabled(true);
        if (!ui->imageInfo->isEnabled())
            ui->imageInfo->enable();
    }
}

void MainWindow::showOptions()
{
    options->show();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::commandLineAddFile(const QString& filenames)
{
    QStringList files = filenames.split("\r\n");
    addFiles(files);
}

void MainWindow::addFiles(QStringList files)
{
    if (!loader.isNull() && loader.data()->isRunning()) emit addLoadFiles(files);
    else
    {
        loader = QSharedPointer<MediaLoader>(new MediaLoader(files));
        connect(loader.data(),
                SIGNAL(results(QVector<QSharedPointer<Media> >, QStringList, QStringList)),
                ui->mediaList,
                SLOT(mediasReceiver(QVector<QSharedPointer<Media> >, QStringList, QStringList)));
        connect(loader.data(),
                SIGNAL(progress(int,int)),
                ui->mediaList,
                SLOT(progressReceiver(int, int)));
        connect(this, SIGNAL(addLoadFiles(QStringList)),
                loader.data(), SLOT(addFiles(QStringList)));
        loader.data()->start();
    }
    this->activateWindow();
}

void MainWindow::closeEvent(QCloseEvent*)
{
    QApplication::quit();
}
