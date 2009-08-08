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


#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H

#include <QObject>
#include "uploadrequest.h"
#include "progresswidget.h"
#include "medialistmodel.h"

class FileUploader : public QObject
{
    Q_OBJECT
public:
    FileUploader(ProgressWidget*, MediaListModel*);
    void setCookie(QString);
    void begin();
    QSharedPointer<Media> current;
private:
    ProgressWidget *progress;
    MediaListModel *medias;
    QSharedPointer<UploadRequest> request;
    int donecount;
    int filecount;
    int failcount;
    int skip;

    QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > > res;
    bool failed;
    void process();
    QString cookie;
    QVector<QSharedPointer<UploadRequest> > tmp ;
    QSharedPointer<QHttp> http;
    void fail(QString reason = NULL);

private slots:
    void progressReceiver(int value);
    void statusReceiver(int value);
    void resultReceiver(QString value);
    void cancel();



signals:
    void status(int); // 0 - begin, 1 - done, 2 - error, 3 - abort;
    void results(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > >);
};
#endif // FILEUPLOADER_H
