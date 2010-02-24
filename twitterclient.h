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


#ifndef TWITTERCLIENT_H
#define TWITTERCLIENT_H

#include <QObject>
#include <QDialog>
#include <QHttp>
#include <QList>
#include <QProgressBar>

#include "gallerycreator.h"
#include "httprequest.h"

class TwitterClient : public QObject
{
    Q_OBJECT
public:
    TwitterClient(QDialog *parent = 0);
    ~TwitterClient();
    void post(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > >, QString, QString, QString, QString, bool showProgressbar = false, QPoint pos = QPoint());
    QDialog bar;
    QProgressBar pbar;

private:
    HTTPRequest http;
    GalleryCreator gallery;
    void showProgressBar(QPoint);
    QMap<int, QString> users;
    QString user;

public slots:
    void post(QString, QString, QString, QString, bool showProgressbar = false, QPoint pos = QPoint());
    void resultReceiver(QString);
    void errorReceiver(QString);

signals:
    void errorHappened();
};

#endif // TWITTERCLIENT_H
