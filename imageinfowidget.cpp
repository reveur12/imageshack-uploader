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


#include "imageinfowidget.h"
#include "ui_imageinfowidget.h"
#include "mediawidget.h"
#include <QDebug>
#include <QMovie>

ImageInfoWidget::ImageInfoWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ImageInfoWidget)
{
    m_ui->setupUi(this);
    setFont(QApplication::font());
    items[""] = 0;
    items["100x100"] = 1;
    items["150x150"] = 2;
    items["320x320"] = 3;
    items["640x640"] = 4;
    items["800x800"] = 5;
    items["1024x1024"] = 6;
    items["1280x1280"] = 7;
    items["1600x1600"] = 8;
    items["resample"] = 9;
    loggedIn = false;
}

ImageInfoWidget::~ImageInfoWidget()
{
    delete m_ui;
}

void ImageInfoWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ImageInfoWidget::enable()
{
    if (m_ui->thumbnail->text().size() == 0)
        m_ui->thumbnail->setEnabled(true);
    if (!(media!=NULL && media.data()->getClass() == "video"))
    {
        m_ui->removeSize->setEnabled(true);
        m_ui->resize->setEnabled(true);
    }
    if (!media.isNull() && loggedIn)
    {
        m_ui->privacyPrivate->setEnabled(true);
        m_ui->privacyPublic->setEnabled(true);
        m_ui->tags->setEnabled(true);
    }
}

void ImageInfoWidget::disable()
{
    media.clear();
    m_ui->thumbnail->setEnabled(false);
    m_ui->thumbnail->setText(tr("Thumbnail"));
    m_ui->removeSize->setEnabled(false);
    m_ui->resize->setEnabled(false);
    m_ui->privacyPrivate->setEnabled(false);
    m_ui->privacyPublic->setEnabled(false);
    m_ui->tags->setEnabled(false);
    m_ui->tags->clear();
    m_ui->resize->setCurrentIndex(0);
    m_ui->removeSize->setChecked(false);
    m_ui->privacyPublic->setChecked(true);
}

void ImageInfoWidget::reset()
{
    media.clear();
    m_ui->thumbnail->setText(tr("Thumbnail"));
    m_ui->thumbnail->setEnabled(false);
    m_ui->removeSize->setEnabled(false);
    m_ui->resize->setEnabled(false);
    m_ui->privacyPrivate->setEnabled(false);
    m_ui->privacyPublic->setEnabled(false);
    m_ui->tags->setEnabled(false);
    m_ui->tags->clear();
    m_ui->resize->setCurrentIndex(0);
    m_ui->removeSize->setChecked(false);
    m_ui->privacyPublic->setChecked(true);
}

void ImageInfoWidget::setMedia(QSharedPointer<Media> item)
{
    media = item;
    m_ui->thumbnail->setEnabled(true);
    m_ui->resize->setCurrentIndex(items[media->getResize()]);
    m_ui->removeSize->setChecked(media->getRemoveSize());
    m_ui->thumbnail->setPixmap(QPixmap::fromImage(media->thumbnail()));
    bool isImage = (media.data()->getClass() == "image");
    m_ui->resize->setEnabled(isImage);
    m_ui->removeSize->setEnabled(isImage);
    m_ui->tags->setText(media.data()->getTags().join(", "));
    m_ui->privacyPrivate->setChecked(media.data()->getPrivate());
    m_ui->privacyPublic->setChecked(!media.data()->getPrivate());
    if (loggedIn)
    {
        m_ui->privacyPrivate->setEnabled(true);
        m_ui->privacyPublic->setEnabled(true);
        m_ui->tags->setText(media.data()->getTags().join(", "));
        m_ui->tags->setEnabled(true);
    }
}

void ImageInfoWidget::setResize(int idx)
{
    if (media)
        media->setResize(items.key(idx));
    if (idx==1 || idx==2)
        m_ui->removeSize->setEnabled(false);
    else
        m_ui->removeSize->setEnabled(true);
}

void ImageInfoWidget::setRemoveSize(int state)
{
    qDebug() << state;
    if (media)
        media->setRemoveSize(state);
}

void ImageInfoWidget::thumbnailClicked()
{
    if (!media || (media.data()->getClass()!="image" &&
                   media.data()->getClass()!="video"))
        return;
    if (m_ui->thumbnail->text().size()!=0) return;
    if (!media.data()->hasPreview()) return;
    MediaWidget *mediaWidget = new MediaWidget();
    mediaWidget->setMedia(media);
    mediaWidget->show();
}


QString ImageInfoWidget::cleanup(QString str)
{
    while ((!str.isNull()) && (str.left(1) == " ")) str.remove(0, 1);
    while ((!str.isNull()) && (str.right(1) == " ")) str.remove(str.length()-1, 1);
    return str;
}

void ImageInfoWidget::tagsUpdated()
{
    if (!media) return;
    QString text = m_ui->tags->text();
    QStringList taglist = text.split(',');
    QStringList result;
    foreach(QString tag, taglist)
    {
        QString newtag = cleanup(tag);
        if (!newtag.isEmpty())
            result.append(newtag);
    }
    media.data()->setTags(result);
}

void ImageInfoWidget::privacyUpdated()
{
    qDebug() << "setting media privacy to" << m_ui->privacyPrivate->isChecked();
    if (media == NULL) return;
    media.data()->setPrivacy(m_ui->privacyPrivate->isChecked());
}

void ImageInfoWidget::batchPrivacy(bool st)
{
    if (st)
    {
        m_ui->privacyPrivate->setChecked(true);
        m_ui->privacyPublic->setChecked(false);
    }
    else
    {
        m_ui->privacyPrivate->setChecked(false);
        m_ui->privacyPublic->setChecked(true);
    }
}

void ImageInfoWidget::loginStatusReceiver(int state)
{
    if (state == 0)
    {
        if (media!=NULL)
        {
            m_ui->privacyPrivate->setEnabled(true);
            m_ui->privacyPublic->setEnabled(true);
            m_ui->tags->setEnabled(true);
        }
        loggedIn = true;
        this->setToolTip(QString());
    }
    else
    {
        m_ui->privacyPrivate->setEnabled(false);
        m_ui->privacyPublic->setEnabled(false);
        m_ui->tags->setEnabled(false);
        loggedIn = false;
        setToolTip(tr("Log in to use this features"));
    }

}
