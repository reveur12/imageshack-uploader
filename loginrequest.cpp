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
#include "defines.h"

LoginRequest::LoginRequest()
{
    connect(&http, SIGNAL(requestFinished(int, bool)),
             this, SLOT(requestFinished(int, bool)));
}

void LoginRequest::login(QString login, QString pass)
{
    username = login;
    password = pass;
    QHttpRequestHeader header("POST", LOGIN_PATH, 1, 1);
    header.addValue("Content-Type","application/x-www-form-urlencoded");
    header.addValue("Cache-Control", "no-cache");
    header.addValue("Host", LOGIN_HOSTNAME);
    header.addValue("Accept","*/*");

    QByteArray data = "username=";
    data.append(QUrl::toPercentEncoding(username));
    data.append("&password=");
    data.append(QUrl::toPercentEncoding(password));
    data.append("&nocookie=1&format=xml");

    http.setHost(LOGIN_HOSTNAME);
    requestId = http.request(header,data);
}

void LoginRequest::requestFinished(int id, bool error)
{
    if (requestId == id)
    {
        if (error) emit failed();
        else
        {
            QDomDocument xml;
            QString data = http.readAll();
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
    }
}

void LoginRequest::abort()
{
    http.abort();
}
