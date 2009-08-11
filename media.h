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


#ifndef MEDIA_H
#define MEDIA_H

#include <QObject>
#include <QImage>
#include <QStringList>

class Media : public QObject
{
    Q_OBJECT
public:
    Media();
    Media(QString filename);
    int width();
    int height();
    qint64 size();
    QImage thumbnail();
    QImage icon();
    QImage make_thumbnail(QImage);
    QImage make_icon();
    QString filepath();
    QString filename();
    QString formatedSize();
    QString formatSize(qint64);
    QString getType();
    QString getClass();
    QImage getPreview();
    QStringList getTags();
    QString getResize();
    bool getRemoveSize();
    bool getPrivate();

    void setTags(QStringList);
    void setPrivacy(bool);
    void setResize(QString);
    void setRemoveSize(bool);
    QByteArray getData();
    bool operator=(const Media other);
    bool isValid();
    bool isTooLarge();
    QByteArray data();
    bool hasPreview();

private:
    int image_width;
    int image_height;
    qint64 filesize;
    QString file;
    QImage ic, thumb;
    QString mediaType;
    QString mediaClass;
    QStringList tags;
    bool privacy;
    QString resize;
    bool removeSize;
    QImage videoPreview;

    bool valid;
    bool toolarge;

};

#endif // MEDIA_H
