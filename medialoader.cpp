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


#include "medialoader.h"
#include <QDebug>

MediaLoader::MediaLoader(QStringList files)
{
    filelist = files;
}

void MediaLoader::run()
{
    QVector<QSharedPointer<Media> > res;
    QStringList toolarge;
    QStringList failed;
    int current = 0;
    qDebug() << current << filelist.count();
    emit progress(current++, filelist.count());
    qDebug() << filelist << filelist.count();
    for(int i = 0; i< filelist.size(); i++)
    {
        QString filename = filelist.at(i);
        QSharedPointer<Media> media = QSharedPointer<Media>(new Media(filename));
        if (media->isValid())
            res.append(media);
        else
        {
            if (media.data()->isTooLarge()) toolarge.append(filename);
            else failed.append(filename);
        }
        emit progress(current++, filelist.count());
    }
    //if (failed.size() || toolarge.size()) emit loadingFailed(failed, toolarge);
    emit results(res, failed, toolarge);
}

void MediaLoader::addFiles(QStringList files)
{
    filelist.append(files);
}
