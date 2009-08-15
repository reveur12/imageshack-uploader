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


#include "uploadrequest.h"
#include <QFile>
#include <QVector>
#include <QPair>
#include <QTextDocument>
#include <QFileInfo>
#include <QHttpRequestHeader>
#include <QBuffer>
#include "defines.h"
#include <QDebug>
#include "filesource.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDataStream>

UploadRequest::UploadRequest(QSharedPointer<Media> smedia, QHttp *)
{
    media = smedia;
    mediaClass = media.data()->getClass();
    mediaType = media.data()->getType();
    optimize = media.data()->getResize();
    isPublic = !(media.data()->getPrivate());
    removeBar = media.data()->getRemoveSize();
    key = DEVELOPER_KEY;
    filename = media.data()->filepath();

    finished = false;
    failed = false;
    aborted = false;
    uploaded = 0;
}

void UploadRequest::setFileName(QString file)
{
    if (QFile(file).exists()) filename = file;
}

void UploadRequest::setOptimize(QString size)
{
    optimize = size;
}

void UploadRequest::setTags(QStringList taglist)
{
    tags = taglist;
}

void UploadRequest::setPublic(bool value)
{
    isPublic = value;
}

void UploadRequest::setAuth(QString auth, QString user, QString pass)
{
    if (!auth.isEmpty()) cookie = auth;
    else
    {
        username = user;
        password = pass;
    }
}

void UploadRequest::setKey(QString value)
{
    key = value;
}

void updateHeaders()
{

}

void UploadRequest::setRemoveBar(bool value)
{
    removeBar = value;
}

QString UploadRequest::post()
{
    QVector<QPair<QString,QString> > fields;
    QString alltags = tags.join(",");
    fields << qMakePair(QString("public"), QString(isPublic?"yes":"no"));
    if (!cookie.isEmpty()) fields << qMakePair(QString("cookie"), cookie);
    if (!alltags.isEmpty()) fields << qMakePair(QString("tags"), alltags);
    if ((mediaClass == "image") || (mediaClass == "application"))
        {
        if (!optimize.isNull())
        {
            fields << qMakePair(QString("optimage"), QString("1"));
            fields << qMakePair(QString("optsize"), optimize);
        }
        if (removeBar) fields << qMakePair(QString("rembar"), QString("1"));
    }
    qDebug() << fields;
    fields << qMakePair(QString("key"), key);

    uploadFile(filename, fields);
    return QString();
}

void UploadRequest::uploadFile(QString, QVector<QPair<QString,QString> > fields)
{
    QString host;
    QString path;

    if ((mediaClass == "image") || (mediaClass == "application"))
    {
        host = "load"+QString::number((qrand()%9)+1)+"." + UPLOAD_HOSTNAME;
        path = UPLOAD_PATH;
    } else
    {
        host = VIDEO_UPLOAD_HOSTNAME;
        path = VIDEO_UPLOAD_PATH;
    }

    QString boundary("UPLOADERBOUNDARY");

    data = QSharedPointer<FileSource>(new FileSource(media, fields));
    data.data()->open(QIODevice::ReadOnly);

    QNetworkRequest req(QUrl("http://" + QString(host) + path));
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  QString("multipart/form-data, boundary=" + boundary) );

    rep = qnam.post(req, data.data());

    connect(rep, SIGNAL(uploadProgress (qint64, qint64)),
            this, SLOT(updateProgress(qint64, qint64)));
    connect(rep, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(uploadFailed(QNetworkReply::NetworkError)));
    connect(rep, SIGNAL(finished()),
            this, SLOT(uploadFinished()));

    emit status(0);
    emit progress(0);
    qDebug() << "Started request";
}

void UploadRequest::uploadFailed(QNetworkReply::NetworkError code)
{
    qDebug() << "upload failed with code " << code;
    if (failed || aborted) return;
    failed = true;
    disconnectReply();
    emit status(2);
}

void UploadRequest::uploadFinished()
{
    qDebug() << "upload finished";
    if (aborted || failed) return;
    QString res = rep->readAll();
    if (res.isEmpty())
    {
        uploadFailed(QNetworkReply::ContentNotFoundError);
        return;
    }
    disconnectReply();
    emit status(1);
    emit result(res);
}

void UploadRequest::updateProgress(qint64 done, qint64 total)
{
    if (total)
    {
        uploaded = media.data()->size() * done / total;
        emit progress(done*100/total);
    }
}

void UploadRequest::stop()
{
    aborted = true;
    emit status(3);
    disconnectReply();
    rep->abort();
}

void UploadRequest::fail()
{
    if (failed) return;
    qDebug() << "UploadRequest failing";
    failed = true;
    disconnectReply();
    emit status(2);
}

void UploadRequest::disconnectReply()
{
    if (rep==NULL) return;
    rep->disconnect(this, SLOT(uploadFinished()));
    rep->disconnect(this, SLOT(updateProgress(qint64,qint64)));
    rep->disconnect(this, SLOT(uploadFailed(QNetworkReply::NetworkError)));
    rep = NULL;
}
