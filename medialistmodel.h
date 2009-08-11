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


#ifndef MEDIALISTMODEL_H
#define MEDIALISTMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QSharedPointer>
#include "media.h"
#include "medialoader.h"

class MediaListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    MediaListModel(QWidget * p = 0);
    void addMedia(QString filename);
    QVariant data(const QModelIndex&, int) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QSharedPointer<Media> getMedia(QModelIndex);
    QSharedPointer<Media> getMedia(int);
    void removeMedia(int row = -1);
    void removeMediaNoDelete(int row = -1);
    void removeAllMedias();
    QVector<QSharedPointer<Media> > getAllMedias();
    int totalSize();
    QString formattedTotalSize();
    void addMedia(QSharedPointer<Media>);
    void addMedia(QVector<QSharedPointer<Media> >);
    void setTags(QStringList);
    QStringList getTags();
    QVector<QSharedPointer<Media> > medias;

    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action, int row,
                      int column, const QModelIndex &parent);

private:

    QStringList tags;
    QSharedPointer<MediaLoader> loader;
    QWidget *parentw;

signals:
    void modelChanged();
    void addLoadFiles(QStringList);
};

#endif // MEDIALISTMODEL_H
