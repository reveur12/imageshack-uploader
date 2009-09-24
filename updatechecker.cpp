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


#include "updatechecker.h"
#include <QMessageBox>
#include <QDebug>
#include <QDomDocument>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMap>
#include <QFile>
#include <QStringList>
#include <QPushButton>
#include <QtAlgorithms>
#include "defines.h"

UpdateChecker::UpdateChecker()
{
    inProgress = false;
    req.connectError(this, SLOT(errorReceiver(QString)));
    req.connectResult(this, SLOT(resultReceiver(QString)));
}

UpdateChecker::~UpdateChecker()
{
    req.stop();
}

void UpdateChecker::check(bool silent)
{
    workSilent = silent;
    if (inProgress)
        QMessageBox::information(NULL, tr("Please wait..."),
                  tr("Update checkup is currently in progress."));
    inProgress = true;

    req.get(QString("http://") + UPDATE_HOSTNAME + UPDATE_PATH);
}

bool versionCompare(QString one, QString two)
{
    QStringList ones = one.split("."); // if versions has different length,
    QStringList twos = two.split("."); // add zeros to shorter version
    while (ones.size()<twos.size()) ones.append("0");
    while (ones.size()>twos.size()) twos.append("0");
    for (int i=0; i<ones.size(); i++)
    {
        QString curone = ones.at(i);
        QString curtwo = twos.at(i);
        if (curone.toInt()>curtwo.toInt()) return true;
        if (curone.toInt()<curtwo.toInt()) return false;
    }
    return false;
}

void UpdateChecker::errorReceiver(QString msg)
{
    QMessageBox::critical(NULL, tr("Error"),
              tr("Could not check for updates. Please try again later.") +
              "\n" + tr("Error message is:") + req.errorString());
    inProgress = false;
    return;
}

void UpdateChecker::resultReceiver(QString data)
{
    QDomDocument xml;

    qDebug() << data;

    xml.setContent(data);
    QDomElement productxml = xml.firstChildElement("product");
    if (productxml.isNull())
    {
        QMessageBox::critical(NULL, tr("Error"),
              tr("Could not check for updates. Wrong server response."));
        inProgress = false;
        return;
    }

    QDomElement urlxml = productxml.firstChildElement("url");

    QString url = urlxml.text();

    QDomNodeList versionsx = xml.elementsByTagName("version");
    if (versionsx.length()==0)
    {
        QMessageBox::critical(NULL, tr("Error"),
              tr("Could not check for updates. No versions found."));
        inProgress = false;
        return;
    }

    QString platform;

#ifdef Q_OS_WIN
    platform = "WIN";
#endif
#ifdef Q_OS_MAC
    platform = "MAC";
#endif
    if (platform.isEmpty()) platform = "UNIX";
    QStringList versions;
    QMap<QString, QUrl> urls;
    for(unsigned int i=0; i<versionsx.length(); i++)
    {
        QDomNode version = versionsx.at(i);
        QDomNode platformx = version.attributes().namedItem("platform");
        if (platformx.isNull()) continue;
        QString vpl = platformx.nodeValue();
        if ((vpl != platform) && (vpl != "ALL")) continue;
        QString ver = version.attributes().namedItem("id").nodeValue();
        versions.append(ver);
        QDomElement xurl = version.toElement().firstChildElement("url");
        urls[ver] = QUrl(xurl.text());
    }
    if (!versions.size())
    {
        if (!workSilent)
        QMessageBox::information(NULL, tr("No updates available"),
         tr("You have the latest version of this application."));
        inProgress = false;
        return;
    }
    qSort( versions.begin(), versions.end(), versionCompare); // I should write Version class with right compare
    QString version = versions.first();                       // operator and use QMap's auto sorting.
    if (versionCompare(version, QApplication::applicationVersion()))
    {
        QMessageBox msg;
        msg.addButton(tr("Yes"), QMessageBox::YesRole);
        msg.addButton(tr("No"), QMessageBox::NoRole);
        msg.setText(tr("Lastest version is %1. Do you want to update?").arg(version));
        msg.setWindowTitle(tr("Update available"));
        msg.exec();
        if (msg.buttonRole(msg.clickedButton()) == QMessageBox::YesRole)
        {
            QDesktopServices().openUrl(urls[version]);
        }
    }
    else
    {
        if (!workSilent)
        {
            QMessageBox::information(NULL, tr("No updates available"),
                       tr("You have the latest version of this application."));
        }
    }
    inProgress = false;
}
