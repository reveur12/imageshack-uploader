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


#include <QDebug>
#include <QUrl>
#include "gallerycreator.h"
#include "defines.h"

GalleryCreator::GalleryCreator()
{
    http.setHost(GALLERY_HOSTNAME);
    connect(&http,
            SIGNAL(requestFinished(int, bool)),
            this,
            SLOT(resultsReceiver(int,bool)));
}

void GalleryCreator::create(QStringList urls, QString text, QString user, QString pass, QString shortlink)
{
    QStringList images;
    foreach(QString url, urls)
    {
        QString image = url.split("imageshack.us/").at(1);
        images.append(image);
    }
    QStringList postlist;
    for(int i=0; i<images.size(); i++)
    {
        postlist.append("imgfile"+QString::number(i+1)+"=" + images.at(i));
    }
    QString post = postlist.join("&");

    qDebug () << post;

    QString first = images.at(0);
    QStringList parts = first.split("/");

    QString path = QString(GALLERY_PATH);
    qDebug() << path;
    QHttpRequestHeader header("POST", path, 1, 1);

    header.addValue("Content-Type","application/x-www-form-urlencoded");
    header.addValue("Cache-Control", "no-cache");
    QString host = parts.first() + "." + GALLERY_HOSTNAME;
    qDebug() << host;
    http.setHost(host);
    header.addValue("Host", host);

    header.addValue("Accept","*/*");
    QByteArray postdata;
    postdata.append(post);
    int id = http.request(header, postdata);
    qDebug() << "sent gallery creation request";
    ids.append(id);
    QStringList d;
    d << text << user << pass << shortlink;
    data[id] = d;
}


void GalleryCreator::resultsReceiver(int id, bool failed)
{
    if (ids.contains(id) && !failed)
    {
        qDebug() << "gallery created";
        ids.removeAll(id);
        emit creationFinished(data[id].at(3), data[id].at(0), data[id].at(1),
                              data[id].at(2));
        data.remove(id);
    }
}
