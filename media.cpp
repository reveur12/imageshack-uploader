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


#include "media.h"
//#include "videoframereader.h"
#include "videopreviewcreator.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>

Media::Media()
{
}

Media::Media(QString filename)
{
    valid = true;
    QFile readfile(filename);
    /*if (readfile.size() > 104857600)
    {
        toolarge = true;
        valid = false;
        return;
    }*/
    toolarge = false;
    valid = readfile.open(QFile::ReadOnly);
    if (!valid) return;
    readfile.close();
    file = filename;
    filesize = QFile(filename).size();
    QMap<QString, QPair<QString, QString> > types;
    types["jpg"] = qMakePair(QString("image"), QString("jpeg"));
    types["jpeg"] = qMakePair(QString("image"), QString("jpeg"));
    types["png"] = qMakePair(QString("image"), QString("png"));
    types["gif"] = qMakePair(QString("image"), QString("gif"));
    types["bmp"] = qMakePair(QString("image"), QString("bmp"));
    types["tiff"] = qMakePair(QString("image"), QString("tiff"));
    types["tif"] = qMakePair(QString("image"), QString("tiff"));
    types["swf"] = qMakePair(QString("application"), QString("x-shockwave-flash"));
    types["pdf"] = qMakePair(QString("application"), QString("pdf"));
    types["flv"] = qMakePair(QString("video"), QString("x-flv"));
    types["mp4"] = qMakePair(QString("video"), QString("mp4"));
    types["wmv"] = qMakePair(QString("video"), QString("x-ms-wmv"));
    types["3gp"] = qMakePair(QString("video"), QString("3gpp"));
    types["avi"] = qMakePair(QString("video"), QString("avi"));
    types["mov"] = qMakePair(QString("video"), QString("quicktime"));
    types["mkv"] = qMakePair(QString("video"), QString("x-matroska"));
    if (filename.split(".").length() < 2)
    {
        valid = false;
        return;
    }
    QString extension = filename.split('.').last().toLower();
    QPair<QString, QString> mimetype;
    if (types.contains(extension))
        mimetype = types[extension];
    else
    {
        valid = false;
        return;
    }
    mediaClass = mimetype.first;
    mediaType = mimetype.second;
    removeSize = false;
    if (mediaClass == "image")
    {
        QImage image;
        valid = image.load(filename);
        if (!valid) return;
        image_width = image.width();
        image_height = image.height();
        thumb = make_thumbnail(image);
        ic = make_icon();
    }
    else if (mediaType == "pdf")
    {
        image_width = image_height = 150;
        thumb = QImage(":/images/images/pdf.jpg");
        ic = thumb.scaledToWidth(25, Qt::SmoothTransformation);
    }
    else if (mediaType == "x-shockwave-flash")
    {
        QImage im(":/images/images/flash.png");
        image_width = image_height = 150;
        thumb = QImage(":/images/images/flash.png");
        ic = thumb.scaledToWidth(25, Qt::SmoothTransformation);
    }
    else if (mediaClass == "video")
    {
        //VideoFrameReader reader;
        VideoPreviewCreator prev;
        QImage img;
        img.loadFromData(prev.getPreview(filename));
        videoPreview = img;
        if (img.isNull())
            img.load(":/images/images/video.png");
        image_width = img.width();
        image_height = img.height();
        thumb = make_thumbnail(img);
        ic = thumb.scaledToWidth(25, Qt::SmoothTransformation);
    }
}

QString Media::getClass()
{
    return mediaClass;
}

QString Media::getType()
{
    return mediaType;
}

int Media::width()
{
    return image_width;
}

int Media::height()
{
    return image_height;
}

qint64 Media::size()
{
    return filesize;
}

QImage Media::thumbnail()
{
    return thumb;
}

QImage Media::icon()
{
    return ic;
}

QImage Media::make_thumbnail(QImage image)
{
    if ((width()<150) && (height()<150)) return image;
    if (width()>height())
        return image.scaledToWidth(150, Qt::SmoothTransformation);
    else
        return image.scaledToHeight(150, Qt::SmoothTransformation);
}

QImage Media::make_icon()
{
    if (thumb.isNull())
    {
        QImage image;
        image.load(filepath());
        thumb = make_thumbnail(image);
    }
    if (width()>height())
        return thumb.scaledToWidth(25, Qt::SmoothTransformation);
    else
        return thumb.scaledToHeight(25, Qt::SmoothTransformation);
}

QString Media::filepath()
{
    return file;
}

QString Media::filename()
{
    return QFileInfo(file).fileName();
}

QImage Media::getPreview()
{
    if (getClass() == "image")
    {
        QImage image;
        image.load(filepath());
        if (width()<800 && height()<600)
            return image;
        return (width()>height())?image.scaledToWidth(800, Qt::SmoothTransformation):
                                  image.scaledToHeight(600, Qt::SmoothTransformation);
    }
    else if (getClass() == "video")
    {
        return videoPreview;
    }
    else return QImage();
}

void Media::setTags(QStringList taglist)
{
    tags = taglist;
}

void Media::setPrivacy(bool value)
{
    privacy = value;
}

QStringList Media::getTags()
{
    return tags;
}

QString Media::getResize()
{
    return resize;
}

bool Media::getRemoveSize()
{
    return removeSize;
}

void Media::setResize(QString tosize)
{
    resize = tosize;
}

void Media::setRemoveSize(bool toremovesize)
{
    removeSize = toremovesize;
}

QByteArray Media::getData()
{
    return data();
}

bool Media::getPrivate()
{
    return privacy;
}

bool Media::operator=(const Media other)
{
    return this->file == other.file;
}

bool Media::isValid()
{
    return valid;
}

bool Media::isTooLarge()
{
    return toolarge;
}

QString Media::formatedSize()
{
    return formatSize(size());
}

QByteArray Media::data()
{
    QFile file(filepath());
    return file.readAll();
}

QString Media::formatSize(qint64 bytes)
{
    qint64 tsize = bytes;
    qint64 tmp = tsize;
    qint64 exp = 0;
    while (tsize>0 && exp<4) { tmp=tsize; exp++; tsize/=1024; }
    QString res;
    if (exp == 1) res = tr("%n B", "", tmp);
    if (exp == 2) res = tr("%n KB", "", tmp);
    if (exp == 3) res = tr("%n MB", "", tmp);
    if (exp == 4) res = tr("%n GB", "", tmp);
    return res;
}

bool Media::hasPreview()
{
    if (getClass() == "video")
    {
        return !videoPreview.isNull();
    }
    else return true;
}
