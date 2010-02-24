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


#include <QApplication>
#include <QSettings>
#include <QStringList>
#include "windowsexplorerintegrator.h"

WindowsExplorerIntegrator::WindowsExplorerIntegrator()
{
}

void WindowsExplorerIntegrator::integrate()
{
    QString exe = QApplication::applicationFilePath();
    exe.replace("/","\\");
    QStringList formats;
    formats << "jpg" << "jpeg" << "png" << "gif" << "bmp" << "tiff"
            << "tif" << "swf" << "pdf" << "mp4" << "wmv" << "3gp"
            << "avi" << "mov" << "mkv";
    // Windows sucks. In registry formats has default handlers, which has
    // context menu associations. So first i have to get handler name
    // and then add item to it's context menu.
    foreach(QString format, formats)
    {
        QSettings reg("HKEY_CLASSES_ROOT\\."+format, QSettings::NativeFormat);
        QString handler = reg.value(".", QVariant("")).toString();
        QSettings reg2("HKEY_CLASSES_ROOT\\"+handler, QSettings::NativeFormat);
        reg2.beginGroup("shell");
        reg2.beginGroup("imageshack");
        reg2.setValue(".", QVariant(tr("Open with ImageShack Uploader")));
        reg2.beginGroup("command");
        reg2.setValue(".", QVariant(exe+" \"%1\""));
        reg2.endGroup();
        reg2.endGroup();
        reg2.endGroup();
    }
}

void WindowsExplorerIntegrator::deintegrate()
{
    QStringList formats;
    formats << "jpg" << "jpeg" << "png" << "gif" << "bmp" << "tiff"
            << "tif" << "swf" << "pdf" << "mp4" << "wmv" << "3gp"
            << "avi" << "mov" << "mkv";
    foreach(QString format, formats)
    {
        QSettings reg("HKEY_CLASSES_ROOT\\."+format, QSettings::NativeFormat);
        QString handler = reg.value(".", QVariant("")).toString();
        QSettings reg2("HKEY_CLASSES_ROOT\\"+handler, QSettings::NativeFormat);
        reg2.beginGroup("shell");
        reg2.remove("imageshack");
        reg2.endGroup();
    }

}
