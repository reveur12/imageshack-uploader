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


#include "medialistwidget.h"
#include "ui_medialistwidget.h"
#include <QFileDialog>
#include <QSettings>
#include <QDesktopServices>
#include <QMessageBox>
#include <QDebug>
#include "defines.h"

MediaListWidget::MediaListWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MediaListWidget)
{
    m_ui->setupUi(this);
    setFont(QApplication::font());

    mediaList = new MediaListModel(this);

    qRegisterMetaType<QVector<QSharedPointer<Media> > >("QVector<QSharedPointer<Media> >");
    m_ui->mediaList->setModel(mediaList);
    connect(m_ui->mediaList->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(mediaSelected(QItemSelection, QItemSelection)));
    connect(mediaList,
            SIGNAL(modelReset()),
            this,
            SLOT(modelReseted()));
    connect(mediaList,
            SIGNAL(modelChanged()),
            this,
            SLOT(modelChanged()));
    updateStats();

    connect((QObject*)m_ui->mediaList->verticalScrollBar(),
            SIGNAL(rangeChanged(int,int)),
            this,
            SLOT(scrollbarUpdate(int, int)));

    hasScrollbar = false;
    m_ui->mediaList->setColumnWidth(0, size().width()-95-hasScrollbar*15);
}

MediaListWidget::~MediaListWidget()
{
    delete m_ui;
}

void MediaListWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        updateStats();
        break;
    default:
        break;
    }
}

void MediaListWidget::addFile()
{
    QSettings sets;
    QString d = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    QString path = sets.value("imagedir", QVariant(d)).toString();
    QStringList files =  QFileDialog::getOpenFileNames(this,
           tr("Select one or more files to open"),
           path,
           //tr("Images (*.jpeg *.jpg *.png *.gif *.tiff *.tif *bmp), Videos (*.avi *.mp4 *.wmv *.3gp *.mkv *.mov), Flash (*.flv *.swf), PDF (*.pdf)"));
           tr("Supported formats (*.jpeg *.jpg *.png *.gif *.tiff *.tif *bmp *.avi *.mp4 *.wmv *.3gp *.mkv *.m4v *.mov *.flv *.swf *.pdf)"));
    if (files.isEmpty()) return;
    path = QFileInfo(files.at(0)).path();
    sets.setValue("imagedir", QVariant(path));
    if (!loader.isNull() && loader.data()->isRunning()) emit addLoadFiles(files);
    else
    {
        loader = QSharedPointer<MediaLoader>(new MediaLoader(files));
        connect(loader.data(),
                SIGNAL(results(QVector<QSharedPointer<Media> >, QStringList, QStringList)),
                this,
                SLOT(mediasReceiver(QVector<QSharedPointer<Media> >, QStringList, QStringList)));
        connect(loader.data(),
                SIGNAL(progress(int,int)),
                this,
                SLOT(progressReceiver(int, int)));
        connect(this,
                SIGNAL(addLoadFiles(QStringList)),
                loader.data(),
                SLOT(addFiles(QStringList)));
        loader.data()->start();
    }
    updateStats();
}

void MediaListWidget::mediasReceiver(QVector<QSharedPointer<Media> > newmedias,
                                     QStringList failed, QStringList toolarge)
{
    bool first = false;
    if ((mediaList->rowCount(QModelIndex())) == 0) first = true;
    if (!newmedias.isEmpty())
    {
        mediaList->addMedia(newmedias);
        updateStats();
        if (mediaList->rowCount())
        if (first)
        {
            QModelIndex index = mediaList->index(0,0,QModelIndex());
            QItemSelectionModel *model = m_ui->mediaList->selectionModel();
            model->select(index, QItemSelectionModel::ToggleCurrent | QItemSelectionModel::Rows);
            m_ui->mediaList->setFocus(Qt::MouseFocusReason);
        }
    }
    QStringList text;
    if (failed.size())
    {
        text.append(tr("Could not add %n file(s), as they seems to be in a format we do not currently support:",
                       0, failed.size()));
        foreach(QString filename, failed)
            text.append(filename);
    }
    if (toolarge.size())
    {
        text.append(tr("%n file(s) are too large for upload:", 0, toolarge.size()));
        foreach(QString filename, toolarge)
            text.append(filename);
        text.append(tr("Maximum file size for logged in users is %n MB", 0,
                       LOGGEDIN_IMAGE_SIZE_LIMIT/1024/1024));
        text.append(tr("Maximum file size for anonymous users is %n MB", 0,
                       NOT_LOGGEDIN_IMAGE_SIZE_LIMIT/1024/1024));
    }
    if (text.size())
        QMessageBox::warning(NULL, tr("Warning"), text.join("\n"));
}

void MediaListWidget::progressReceiver(int current, int total)
{
    if (current==0)
        emit requireProgressbar(true);
    emit progress(current, total);
    if (current==total)
        emit requireProgressbar(false);
}

void MediaListWidget::removeFile()
{
    QModelIndexList indexes = m_ui->mediaList->selectionModel()->selectedIndexes();
    if (indexes.size())
    {
        foreach(QModelIndex index, indexes)
            if (index.column() == 0) mediaList->removeMedia(index.row());
    }
    else
    {
        if (mediaList->rowCount())
        {
            mediaList->removeMedia(mediaList->rowCount()-1);
        }
    }
    updateStats();
}

QSharedPointer<Media> MediaListWidget::getSelectedMedia()
{
    foreach(QModelIndex index,
            m_ui->mediaList->selectionModel()->selectedIndexes())
    return mediaList->getMedia(index);
    return QSharedPointer<Media>();
}

void MediaListWidget::removeAllFiles()
{
    mediaList->removeAllMedias();
    updateStats();
}

void MediaListWidget::mediaSelected(QItemSelection selected, QItemSelection)
{
    if (selected.indexes().size() == 0) return;
    emit mediaSelected(mediaList->getMedia(selected.indexes()[0]));
}

void MediaListWidget::modelReseted()
{
    updateStats();
    emit modelReset();
}

void MediaListWidget::modelChanged()
{
    updateStats();
    emit mediasChanged();
    if (mediaList->rowCount(QModelIndex())==0)
    {
        m_ui->remove->setEnabled(false);
        m_ui->clear->setEnabled(false);
    }
    else
    {
        m_ui->remove->setEnabled(true);
        m_ui->clear->setEnabled(true);
    }
}

void MediaListWidget::resizeEvent ( QResizeEvent * )
{
#ifdef Q_OS_MAC
    m_ui->mediaList->setColumnWidth(0, size().width()-100-hasScrollbar*15);
#endif
#ifdef Q_OS_WIN
    m_ui->mediaList->setColumnWidth(0, size().width()-70-hasScrollbar*15);
#endif
#ifdef Q_WS_X11
    m_ui->mediaList->setColumnWidth(0, size().width()-75-hasScrollbar*15);
#endif
    m_ui->mediaList->resizeColumnToContents(1);
    //m_ui->mediaList->setColumnWidth(1, 10);
}

void MediaListWidget::updateStats()
{
    int count = mediaList->rowCount(QModelIndex());
    m_ui->filecount->setText(tr("%n files", "", count));
    m_ui->size->setText(mediaList->formattedTotalSize());
    if (count==0)
    {
        m_ui->remove->setEnabled(false);
        m_ui->clear->setEnabled(false);
    }
    else
    {
        m_ui->remove->setEnabled(true);
        m_ui->clear->setEnabled(true);
    }
}

QVector<QSharedPointer<Media> > MediaListWidget::getAllMedias()
{
    return mediaList->getAllMedias();
}

MediaListModel* MediaListWidget::getModel()
{
    return mediaList;
}

void MediaListWidget::setEnabled(bool state)
{
    QWidget::setEnabled(state);
    if (!state)
    {
        m_ui->mediaList->clearSelection();
        QItemSelectionModel *model = m_ui->mediaList->selectionModel();
    }
}

void MediaListWidget::scrollbarUpdate(int a, int b)
{
    if (a==0 && b==0) hasScrollbar = false;
    else hasScrollbar = true;
    resizeEvent(0);
}

void MediaListWidget::selectFirst()
{
    if (!mediaList->rowCount()) return;
    QModelIndex index = mediaList->index(0,0,QModelIndex());
    QItemSelectionModel *model = m_ui->mediaList->selectionModel();
    model->select(index, QItemSelectionModel::Clear);
    model->select(index, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    m_ui->mediaList->setFocus(Qt::MouseFocusReason);
}

void MediaListWidget::resizeAllImagesTo(QString value)
{
    for (int i=0; i<mediaList->rowCount(); i++)
        if (mediaList->getMedia(i).data()->getClass() == "image")
            mediaList->getMedia(i).data()->setResize(value);
}
