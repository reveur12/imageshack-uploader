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


#ifndef UPLOADREQUEST_H
#define UPLOADREQUEST_H

#include <QObject>
#include <QPair>
#include <QStringList>
#include <media.h>
#include <QHttpResponseHeader>
#include <QSharedPointer>
#include <QTimer>

class UploadRequest : public QObject
{
    Q_OBJECT
public:
    UploadRequest(QSharedPointer<Media>, QHttp*);

    bool finished;

    void setFileName(QString);
    void setOptimize(QString size);
    void setTags(QStringList);
    void setPublic(bool);
    void setAuth(QString auth = NULL, QString user = NULL,
                 QString pass = NULL);
    void setKey(QString);
    void setType(QString classOfMedia, QString typeOfMedia);
    void setRemoveBar(bool value);
    QString post();
    void stop();

private:
    QString mediaClass;
    QString mediaType;
  /*"video/mp4",
    "wmv"       => "video/x-ms-wmv",
    "3gp"       => "video/3gpp",
    "avi"       => "video/avi",
    "mov"       => "video/quicktime",
    "mkv"       => "video/x-matroska"

    "jpg"       => "image/jpeg",
    "jpeg"      => "image/jpeg",
    "png"       => "image/png",
    "gif"       => "image/gif",
    "bmp"       => "image/bmp",
    "tiff"      => "image/tiff",
    "tif"       => "image/tiff",
    "swf"       => "application/x-shockwave-flash",
    "pdf"       => "application/pdf", */
    QString filename;
    QString optimize;
    QStringList tags;
    bool isPublic;
    QString cookie;
    QString username;
    QString password;
    bool removeBar;
    QString key;

    void updateHeaders();
    void uploadFile(QByteArray, QString, QVector<QPair<QString, QString> >);
    QString hostname;

    QByteArray data;


    QHttp realhttp;
    QHttp *http;
    int reqid;
    bool failed;
    bool aborted;
    void fail();

    QTimer connectTimer, answerTimer;

public slots:
    void headerReceiver(QHttpResponseHeader);
    void requestFinished ( int id, bool error );
    void updateProgress(int, int);
    void checkConnect();
    void checkAnswer();
    void stateReceiver(int);

signals:
    void progress(int);
    void status(int); // 0 - begin; 1 - done; 2 - failed; 3 - aborted;
    void result(QString);
};

#endif // UPLOADREQUEST_H
