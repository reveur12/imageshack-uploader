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
#include <QVBoxLayout>
#include "twitterclient.h"
#include "defines.h"

TwitterClient::TwitterClient(QDialog *parent)
{
    http.connectResult(this, SLOT(resultReceiver(QString)));
    http.connectError(this, SLOT(errorReceiver(QString)));
    connect(&gallery,
            SIGNAL(result(QString, QString, QString, QString)),
            this,
            SLOT(post(QString, QString, QString, QString)));

    bar.setParent(parent);
    bar.setWindowTitle(tr("Posting to twitter..."));

    bar.setFixedSize(300, 15);

    QVBoxLayout *l = new QVBoxLayout();
    bar.setLayout(l);
    l->setMargin(0);
    bar.setContentsMargins(0, 0, 0, 0);

    pbar.setStyle(new QPlastiqueStyle());
    pbar.setValue(0);
    pbar.setMaximum(0);
    pbar.setAlignment(Qt::AlignCenter);
    pbar.setFormat(tr("Posting to twitter..."));
    l->addWidget(&pbar);

}

TwitterClient::~TwitterClient()
{
    http.stop();
}

void TwitterClient::post(QString url, QString text, QString user, QString pass, bool showProgressbar, QPoint pos)
{
    qDebug() << "posting url" << url << "and txt" << text;
    this->user = user;
    QVector<QPair<QString, QString> > data;
    data.append(qMakePair(QString("username"), user));
    data.append(qMakePair(QString("password"), pass));
    data.append(qMakePair(QString("message"), text));
    data.append(qMakePair(QString("url"), url));
    data.append(qMakePair(QString("key"), QString(DEVELOPER_KEY)));
    qDebug() << data;
    http.post(QString("http://") + TWITTER_HOST + TWITTER_PATH, data);

    if (showProgressbar) showProgressBar(pos);
}

void TwitterClient::post(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > > medias, QString text, QString user, QString pass, QString shortlink, bool showProgressbar, QPoint pos)
{
    if (showProgressbar) showProgressBar(pos);
    gallery.create(medias, text, user, pass);
}

void TwitterClient::errorReceiver(QString msg)
{
    bar.hide();
    QMessageBox::critical(NULL, tr("Error"),
                          tr("Failed to update twitter status."));
}

void TwitterClient::resultReceiver(QString data)
{
    bar.hide();
    qDebug() << data;
    QDomDocument xml;
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
    QString addr("http://twitter.com/%1/status/%2");
    addr = addr.arg(user, statusid.text());
    QDesktopServices().openUrl(QUrl(addr));
}

void TwitterClient::showProgressBar(QPoint pos)
{
    qDebug() << pos;
    int x = pos.x() - bar.width()/2;
    int y = pos.y() - bar.height()/2;
    bar.move(x, y);
    bar.show();
    bar.raise();
    bar.activateWindow();
}
