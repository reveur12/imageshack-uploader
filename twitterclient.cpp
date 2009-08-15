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


#include <QUrl>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QDomDocument>
#include <QDesktopServices>
#include <QPlastiqueStyle>
#include <QApplication>
#include <QDesktopWidget>
#include <QThread>
#include "twitterclient.h"
#include "defines.h"

TwitterClient::TwitterClient(QWidget *parent)
{
    http.setHost(TWITTER_HOST);
    connect(&http,
            SIGNAL(requestFinished(int, bool)),
            this,
            SLOT(requestFinished(int, bool)));
    connect(&gallery,
            SIGNAL(creationFinished(QString, QString, QString, QString)),
            this,
            SLOT(post(QString, QString, QString, QString)));
    int w = QApplication::desktop()->geometry().width();
    int h = QApplication::desktop()->geometry().height();
    bar.setParent(parent);
    bar.move(w/2-150, h/2-10);
    bar.setWindowTitle(tr("Posting to twitter..."));
    //bar.setEnabled(true);
    //bar.setWindowFlags(Qt::Tool);
    bar.setStyle(new QPlastiqueStyle());
    bar.setFixedSize(300, 15);
    bar.setValue(0);
    bar.setMaximum(0);
    bar.setAlignment(Qt::AlignCenter);
}

TwitterClient::~TwitterClient()
{
    http.disconnect(this, SLOT(requestFinished(int, bool)));
    http.abort();
}

void TwitterClient::post(QString url, QString text, QString user, QString pass)
{
    QHttpRequestHeader header("POST", TWITTER_PATH, 1, 1);
    header.addValue("Content-Type","application/x-www-form-urlencoded");
    header.addValue("Cache-Control", "no-cache");
    header.addValue("Host",TWITTER_HOST);
    header.addValue("Accept","*/*");

    QByteArray postdata;
    postdata.append(QString("url="));
    postdata.append(QUrl::toPercentEncoding(url));

    if (user.isEmpty() || pass.isEmpty())
    {
        QMessageBox::critical(NULL, tr("Error"),
                              tr("First set twitter username and password in options"));
        return;
    }
    postdata.append(QString("&username="));
    postdata.append(QUrl::toPercentEncoding(user));
    postdata.append(QString("&password="));
    postdata.append(QUrl::toPercentEncoding(pass));
    postdata.append(QString("&message="));
    postdata.append(QUrl::toPercentEncoding(text));
    postdata.append(QString("&key=") + QString(DEVELOPER_KEY));

    int id = http.request(header, postdata);
    ids.append(id);

    bar.show();
    bar.raise();
    bar.activateWindow();
}

void TwitterClient::post(QStringList urls, QString text, QString user, QString pass, QString shortlink)
{
    gallery.create(urls, text, user, pass, shortlink);
}

void TwitterClient::requestFinished(int id, bool failed)
{
    qDebug() << id << failed;
    if (ids.contains(id))
    {
        bar.hide();
        ids.removeAll(id);
        if (failed)
        {
            QMessageBox::critical(NULL, tr("Error"),
                                  tr("Could not send url to Twitter"));
            emit errorHappened();
        }
        else
        {
            QDomDocument xml;
            QString data = http.readAll();
            xml.setContent(data);
            QDomElement rsp = xml.firstChildElement("rsp");
            if (rsp.isNull())
            {
                QMessageBox::critical(NULL, tr("Error"),
                      tr("Could not post to twitter. Wrong server response."));
                emit errorHappened();
                return;
            }
            QDomElement error = rsp.firstChildElement("err");
            if (!error.isNull())
            {/*
                1002 media not found
                2005 media is too big
                2001 invalid action specified
                2004 invalid developer key
                1001 empty/invalid username/password
                2002 failed to upload media
                2003 failed to update status*/
                QDomNode codex = error.attributes().namedItem("code");
                if (!codex.isNull())
                {
                    QString code = codex.nodeValue();
                    if (code == "1001")
                    {
                        QMessageBox::critical(NULL, tr("Error"),
                          tr("Could not post to twitter. Wrong credentials."));
                        emit errorHappened();
                        return;
                    }
                    else if (code == "2003")
                    {
                        QMessageBox::critical(NULL, tr("Error"),
                          tr("Failed to update twitter status."));
                        emit errorHappened();
                        return;
                    }
                }
                QMessageBox::critical(NULL, tr("Error"),
                          tr("Could not post to twitter. Internal error."));
                return;
            }
            QDomElement statusid = rsp.firstChildElement("statusid");
            if (statusid.isNull())
            {
                QMessageBox::critical(NULL, tr("Error"),
                      tr("Could not post to twitter. Wrong server response."));
                return;
            }
            QSettings sets;
            QByteArray encu = sets.value("twitteruser", QVariant("")).toByteArray();
            QString user = QByteArray::fromBase64(encu);
            QString addr("http://twitter.com/%1/status/%2");
            addr = addr.arg(user, statusid.text());
            QDesktopServices().openUrl(QUrl(addr));
        }
    }
}
