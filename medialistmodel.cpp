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


#include "medialistmodel.h"
#include <QDebug>
#include <QStringListModel>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QSet>
#include <QSharedPointer>
#include <QMessageBox>

MediaListModel::MediaListModel(QWidget *p)
{
    parentw = p;
}

QVariant MediaListModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const
{
    if (!index.isValid()) return QVariant();
    if (role == Qt::DisplayRole)
    {
        QSharedPointer<Media> im = medias.at(index.row());
        if (index.column() == 0)
        {
            return QVariant(im.data()->filename());
        }
        else if (index.column() == 1)
        {
            return QVariant(im.data()->formatedSize());
        }
    }
    else if ((role == Qt::DecorationRole) and (index.column() == 0))
    {
        QSharedPointer<Media> im = medias.at(index.row());
        return QVariant(im.data()->icon());
    }
    else if((role == Qt::SizeHintRole) and (index.column() == 0))
    {
        return QVariant(QSize(25,25));
    }
    return QVariant();
}

QModelIndex MediaListModel::index(int row, int column, const QModelIndex&) const
{
    if ((column>-1) && (column<2) && (row<medias.size()))
        return createIndex(row, column, medias.at(row).data());
    else return QModelIndex();
}

QModelIndex MediaListModel::parent ( const QModelIndex & ) const
{
    return QModelIndex();
}

int MediaListModel::columnCount ( const QModelIndex & ) const
{
    return 2;
}

int MediaListModel::rowCount ( const QModelIndex & parent) const
{
    if (parent.isValid()) return 0;
    return medias.size();
}

void MediaListModel::addMedia(QString filename)
{
    QSharedPointer<Media> im = QSharedPointer<Media>(new Media(filename));
    if (!im.data()->isValid()) return;
    beginInsertRows(QModelIndex(), medias.size(), medias.size());
    medias.append(im);
    endInsertRows();
    emit modelChanged();
}

void MediaListModel::addMedia(QSharedPointer<Media> media)
{
    beginInsertRows(QModelIndex(), medias.size(), medias.size());
    medias.append(media);
    endInsertRows();
    emit modelChanged();
}

void MediaListModel::addMedia(QVector<QSharedPointer<Media> > newmedias)
{
    int was = 0;
    QVector<int> wasIndexes;
    QStringList uniq;
    QSharedPointer<Media> m1, m2;
    for (int i=0; i<newmedias.size(); i++)
    {
        bool curWas = false;
        for (int j=0; j<medias.size(); j++)
        {
            if (newmedias.at(i).data()->filepath() == medias.at(j).data()->filepath())
            {
                was ++;
                wasIndexes.append(i);
                curWas = true;
                break;
            }
        }
        if (!curWas)
        {
            if (uniq.contains(newmedias.at(i).data()->filepath()))
            {
                was ++;
                wasIndexes.append(i);
            }
            else
            {
                uniq.append(newmedias.at(i).data()->filepath());
            }
        }
    }
    if (was != newmedias.size())
    {
        int toAdd = newmedias.size() - 1 - was;
        beginInsertRows(QModelIndex(), medias.size(),
                        medias.size() + toAdd);
        for(int i=0; i< newmedias.size(); i++)
        {
            if (!wasIndexes.contains(i))
            {
                medias.append(newmedias.at(i));
            }
        }
        endInsertRows();
        emit modelChanged();
    }
    if (was)
    {
        QStringList same;
        foreach(int i, wasIndexes)
            same.append(newmedias.at(i).data()->filepath());
        QMessageBox::information(parentw, tr("Files not added"),
                                 tr("This files are already in list, they were not added again:\n", 0, same.size())
                                 + same.join("\n"));
    }
}

void MediaListModel::removeMedia(int row)
{
    qDebug() << "call";
    if ((row<rowCount()) && (row>=0))
    {
        beginRemoveRows(QModelIndex(), row, row);
        medias.remove(row, 1);
        endRemoveRows();
    }
    emit modelChanged();
    if (!rowCount()) reset();
}

void MediaListModel::removeAllMedias()
{
    foreach (QSharedPointer<Media> m, medias)
        m.clear();
    medias.clear();
    reset();
    emit modelChanged();
}

QVector<QSharedPointer<Media> > MediaListModel::getAllMedias()
{
    return medias;
}

QSharedPointer<Media> MediaListModel::getMedia(QModelIndex index)
{
    return medias.at(index.row());
}

QSharedPointer<Media> MediaListModel::getMedia(int index)
{
    return medias.at(index);
}

qint64 MediaListModel::totalSize()
{
    qint64 total = 0;
    foreach(QSharedPointer<Media> media, medias) total += media.data()->size();
    return total;
}

qint64 MediaListModel::totalUploadSize()
{
    qint64 total = 0;
    foreach(QSharedPointer<Media> media, medias)
    {
        total += media.data()->size();
        total -= media.data()->uploadedSize;
    }
    return total;
}

QString MediaListModel::formattedTotalSize()
{
    return Media().formatSize(totalSize(), true);
}

void MediaListModel::setTags(QStringList taglist)
{
    tags = taglist;
    for(int i=0; i<medias.size(); i++)
        medias.at(i).data()->setBatchTags(tags);
}

QStringList MediaListModel::getTags()
{
    return tags;
}


Qt::DropActions MediaListModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags MediaListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList MediaListModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";
    return types;
}

bool MediaListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                  int, int, const QModelIndex &)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("text/uri-list"))
        return false;
    QStringList filelist;
    QStringList filters;
    foreach(QString type, Media().types.keys())
        filters.append(QString("*.") + type);
    foreach (QUrl filename, data->urls())
    {
        QString localfilename = filename.toLocalFile();
        if (localfilename.isEmpty()) continue;
        if (QFileInfo(localfilename).isDir())
            foreach(QString file, QDir(localfilename).entryList(filters))
                filelist.append(QDir(localfilename).absoluteFilePath(file));
        else filelist.append(filename.toLocalFile());
    }
    if (filelist.isEmpty()) return false;
    if (!loader.isNull() && loader.data()->isRunning()) emit addLoadFiles(filelist);
    else
    {
        loader = QSharedPointer<MediaLoader>(new MediaLoader(filelist));
        connect(loader.data(),
                SIGNAL(results(QVector<QSharedPointer<Media> >, QStringList, QStringList)),
                parentw,
                SLOT(mediasReceiver(QVector<QSharedPointer<Media> >, QStringList, QStringList)));
        connect(loader.data(), SIGNAL(progress(int,int)),
                parentw, SLOT(progressReceiver(int, int)));
        connect(this, SIGNAL(addLoadFiles(QStringList)),
                loader.data(), SLOT(addFiles(QStringList)));
        loader.data()->start();
    }
    return true;
}

