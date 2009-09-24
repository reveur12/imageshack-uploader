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


#include "loginrequest.h"
#include <QFile>
#include <QHttpRequestHeader>
#include <QTextDocument>
#include <QDebug>
#include <QUrl>
#include <QDomDocument>
#include <QMessageBox>
#include "defines.h"

LoginRequest::LoginRequest()
{
    http.connectError(this, SLOT(errorReceiver(QString)));
    http.connectResult(this, SLOT(resultReceiver(QString)));
}

LoginRequest::~LoginRequest()
{
    http.stop();
}

void LoginRequest::login(QString login, QString pass)
{
    aborted = false;
    username = login;
    password = pass;
    QVector<QPair<QString, QString> > fields;
    fields.append(qMakePair(QString("username"), login));
    fields.append(qMakePair(QString("password"), pass));
    fields.append(qMakePair(QString("nocookie"), QString("1")));
    fields.append(qMakePair(QString("format"), QString("xml")));
    QString url = "http://";
    url += LOGIN_HOSTNAME;
    url += LOGIN_PATH;
    http.post(url, fields);
}

void LoginRequest::errorReceiver(QString msg)
{
    qDebug() << http.errorString();
    emit failed();
}

void LoginRequest::resultReceiver(QString data)
{
    if (aborted) return;
    QDomDocument xml;
    xml.setContent(data);
    QDomElement doc = xml.documentElement();
    QDomElement error = doc.firstChildElement("error");
    if (!error.isNull()) { emit wrongPassword(); return; }
    QDomElement cookie = doc.firstChildElement("cookie");
    QDomElement name = doc.firstChildElement("username");
    if (!cookie.text().isNull() && !name.text().isNull())
        emit success(cookie.text(), name.text());
    else emit failed();
}


void LoginRequest::abort()
{
    aborted = true;
    http.stop();
}
