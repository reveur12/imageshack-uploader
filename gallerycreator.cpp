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
#include <QDomDocument>
#include <QDomElement>
#include "gallerycreator.h"
#include "defines.h"

GalleryCreator::GalleryCreator()
{
    http.connectResult(this, SLOT(resultReceiver(QString)));
    http.connectError(this, SLOT(errorReceiver(QString)));
}

void GalleryCreator::create(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > > medias, QString t, QString u, QString p)
{
    text = t; user = u; pass = p;
    QVector<QPair<QString, QString> > data;
    data.append(qMakePair(QString("action"), QString("create")));
    //foreach(QString url, urls)
    for(int i=0; i < medias.data()->size(); i++)
    {
        QString url = medias.data()->at(i).second.at(0);
        QString image = url.split("imageshack.us/").at(1);
        int width = medias.data()->at(i).first.data()->width();
        int height = medias.data()->at(i).first.data()->height();
        QString thumb;
        if (width > 200 || height > 200) thumb = "y"; else thumb = "n";
        data.append(qMakePair(QString("image[]"), image + ";" + QString::number(width) + ";" + QString::number(height) + ";" + thumb));
    }
    qDebug() << data;
    http.post(QString("http://") + GALLERY_HOSTNAME + GALLERY_PATH, data);
}

void GalleryCreator::resultReceiver(QString data)
{
    QDomDocument xml;
    xml.setContent(data);
    QDomElement doc = xml.documentElement();
    QDomElement links = doc.firstChildElement("links");
    QString url = links.firstChildElement("yfrog_url").text();
    emit result(url, text, user, pass);
}

void GalleryCreator::errorReceiver(QString message)
{
    qDebug() << "Gallery creator got error:" << message;
    //qDebug() << "Response text:" << http.reply->readAll();
    emit error();
}
