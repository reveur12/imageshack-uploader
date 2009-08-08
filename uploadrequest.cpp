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
    http = &realhttp;//new QHttp();
    mediaClass = media.data()->getClass();
    mediaType = media.data()->getType();
    optimize = media.data()->getResize();
    isPublic = !(media.data()->getPrivate());
    removeBar = media.data()->getRemoveSize();
    key = DEVELOPER_KEY;
    hostname = UPLOAD_HOSTNAME;
    filename = media.data()->filepath();

    finished = false;
    failed = false;
    aborted = false;

    connectTimer.setSingleShot(true);
    answerTimer.setSingleShot(true);

    connect(http,
            SIGNAL( responseHeaderReceived(QHttpResponseHeader) ),
            this,
            SLOT(headerReceiver(QHttpResponseHeader)));
    connect(http, SIGNAL(requestFinished(int, bool)),
             this, SLOT(requestFinished(int, bool)));
    connect(http, SIGNAL(dataSendProgress ( int, int )),
            this, SLOT(updateProgress(int, int)));
    connect(http, SIGNAL(stateChanged(int)),
            this,
            SLOT(stateReceiver(int)));
    connect(&connectTimer,
            SIGNAL(timeout()),
            this,
            SLOT(checkConnect()));
    connect(&answerTimer,
            SIGNAL(timeout()),
            this,
            SLOT(checkAnswer()));
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
    QString alltags = Qt::escape(tags.join(","));
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

    uploadFile(data, filename, fields);
    return QString();
}

void UploadRequest::uploadFile(QByteArray image,
                               QString filename,
                               QVector<QPair<QString,QString> > fields)
{
    QString host;
    if ((mediaClass == "image") || (mediaClass == "application"))
        host = "load"+QString::number((qrand()%9)+1)+"." + UPLOAD_HOSTNAME;
    else
        host = VIDEO_UPLOAD_HOSTNAME;
    qDebug() << host;
    QString boundary("UPLOADERBOUNDARY");
    //QString nl = "\r\n";

    QHttpRequestHeader header("POST", UPLOAD_PATH, 1, 1);
    header.addValue("Content-type",
                    "multipart/form-data, boundary=\"" + boundary+"\"");
    header.addValue("Cache-Control", "no-cache");
    header.addValue("Host", host);
    header.addValue("Accept","*/*");
/*
    //creating post data
    QByteArray bytes;

    bytes.append("--"); bytes.append(boundary); bytes.append(nl);
    bytes.append("Content-Disposition: ");
    bytes.append(QString("form-data; name=\"%1\"; filename=\"%2\"")
                 .arg(QString("fileupload"), QString(filename.toUtf8())));
    bytes.append(nl);
    bytes.append(QString("Content-Type: %1/%2").arg(mediaClass, mediaType));
    bytes.append(nl);
    bytes.append(nl);
    bytes.append(image);
    bytes.append(nl);

    QPair<QString, QString> field;
    foreach(field, fields)
    {
        bytes.append("--"); bytes.append(boundary); bytes.append(nl);
        bytes.append("Content-Disposition: ");
        bytes.append(QString("form-data; name=\"%1\"").arg(field.first));
        bytes.append(nl);
        bytes.append(nl);
        bytes.append(field.second); bytes.append(nl);
    }

    bytes.append("--");
    bytes.append(boundary);
    bytes.append(nl);*/

    FileSource *data = new FileSource(media, fields);
    data->open(QIODevice::ReadOnly);
    /*QFile f("/home/a2k/test.txt");
    f.open(QIODevice::WriteOnly);
    f.write(data->readAll());
    return;
    qDebug() << data->size();*/

    //qDebug() << QString(host)+UPLOAD_PATH;
    QNetworkRequest req(QUrl("http://" + QString(host) + UPLOAD_PATH));
    //req.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(data->realSize()));
    req.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data, boundary=" + boundary) );


    //data->readAll();

    //QByteArray *realdata = new QByteArray;
    //realdata->append(data->readAll());
    //QByteArray res = data->readAll();
    //realdata->append(res);
    //return;
    rep = qnam.post(req, data);//data);

    connect(rep,
            SIGNAL(uploadProgress (qint64, qint64)),
            this,
            SLOT(updateProgress(qint64, qint64)));
    connect(rep,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(uploadFailed(QNetworkReply::NetworkError)));
    connect(rep,
            SIGNAL(finished()),
            this,
            SLOT(uploadFinished()));

    header.setContentLength(data->size());


    http->setHost(host);
    qDebug() << "starting upload";
    //reqid = http->request(header, data);
    qDebug() << "Emiting status";
    emit status(0);
    emit progress(0);
    qDebug() << "Started request";
}

void UploadRequest::uploadFailed(QNetworkReply::NetworkError code)
{
    qDebug() << "upload failed with code " << code;
    if (failed) return;
    failed = true;
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
    emit status(1);
    emit result(res);
}

void UploadRequest::headerReceiver(QHttpResponseHeader head)
{
    if (aborted || failed) return;
    if (!head.isValid() || (head.statusCode() != 200))
    {
        qDebug() << "got not 200 status code";
        fail();
    }
}

void UploadRequest::requestFinished ( int id, bool error )
{
    qDebug() << "got finish" << id << error<< reqid;
    if (aborted) return;
    if ((id == reqid) && (!failed) && (!aborted))
    {
        if (error) { fail(); return;}
        else
        {
            QString res = http->readAll();
            if (res.isEmpty()) { fail(); return; }
            emit status(1);
            emit result(res);
        }
    }
}

void UploadRequest::updateProgress(qint64 done, qint64 total)
{
//    qDebug() << "Progress:" << done << total;
    if (done == total) answerTimer.start(10000);
    if (total)
    emit progress(done*100/total);
}

void UploadRequest::updateProgress(int done, int total)
{
//    qDebug() << "Progress:" << done << total;
    if (done == total) answerTimer.start(10000);
    if (total)
    emit progress(done*100/total);
}

void UploadRequest::stop()
{
    aborted = true;
    emit status(3);
    http->abort();
}

void UploadRequest::fail()
{
    if (failed) return;
    qDebug() << "UploadRequest failing";
    failed = true;
    qDebug() << http->state();
    if (http->state()!=0) http->abort();
    emit status(2);
}

void UploadRequest::checkConnect()
{
    qDebug() << "connection status check";
    if ( (http->state() == QHttp::HostLookup) ||
         (http->state() == QHttp::Connecting) )
    {
        fail();
    }
}

void UploadRequest::checkAnswer()
{
    qDebug() << "answer status check";
    if ( (http->state() != QHttp::Closing) &&
         (http->state() != QHttp::Unconnected) )
    {
        fail();
    }
}

void UploadRequest::stateReceiver(int state)
{
    qDebug() << state;
    if (state == QHttp::Connecting)
        connectTimer.start(5000);
    if (state == QHttp::Reading)
        answerTimer.start(5000);
}
