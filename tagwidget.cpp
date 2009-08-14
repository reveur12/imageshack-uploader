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


#include "tagwidget.h"
#include "ui_tagwidget.h"
#include <QDebug>
#include <QSharedPointer>

TagWidget::TagWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::TagWidget)
{
    m_ui->setupUi(this);
    medias = NULL;
    setToolTip(tr("Log in to use this features"));
    setFont(QApplication::font());
}

TagWidget::~TagWidget()
{
    delete m_ui;
}

void TagWidget::changeEvent(QEvent *e)
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

QString cleanup(QString str)
{
    while ((!str.isNull()) && (str.left(1) == " ")) str.remove(0, 1);
    while ((!str.isNull()) && (str.right(1) == " ")) str.remove(str.length()-1, 1);
    return str;
}

void TagWidget::batchTagsUpdated()
{
    if (!medias) return;
    QString text = m_ui->batchTags->text();
    QStringList taglist = text.split(',');
    QStringList result;
    foreach(QString tag, taglist)
    {
        QString newtag = cleanup(tag);
        if (!newtag.isEmpty())
            result.append(newtag);
    }
    medias->setTags(result);
}

void TagWidget::loginStatusReceiver(int state)
{
    if (state == 0)
    {
        loggedIn = true;
        if (media!=NULL && enabled)
            setEnabled(true);
        this->setToolTip(QString());
    }
    else
    {
        loggedIn = false;
        setEnabled(false);
        setToolTip(tr("Log in to use this features"));
    }

}

void TagWidget::setMediaList(MediaListModel* model)
{
    medias = model;
}

void TagWidget::setMedia(QSharedPointer<Media> item)
{
    media = item;
    if (loggedIn) setEnabled(true);
    //m_ui->privacyPrivate->setChecked(media.data()->getPrivate());
    //m_ui->privacyPublic->setChecked(!media.data()->getPrivate());
}

void TagWidget::unsetMedia()
{
    media = QSharedPointer<Media>();
}

void TagWidget::unsetMediaList()
{
    medias = NULL;
}

void TagWidget::batchPrivacyUpdated()
{
    qDebug() << "updating all medias privacy to" << m_ui->bPrivacyPrivate->isChecked();
    if (medias==NULL) return;
    if (medias->rowCount()==0) return;
    for (int i=0; i< medias->rowCount(); i++)
    {
        medias->getMedia(i).data()->setPrivacy(m_ui->bPrivacyPrivate->isChecked());
    }
    emit batchPrivacy(m_ui->bPrivacyPrivate->isChecked());
}

void TagWidget::setEnabled(bool st)
{
    QWidget::setEnabled(st);
    if (!st) m_ui->batchTags->clear();
}

void TagWidget::enable()
{
    if (loggedIn && medias && medias->rowCount()) setEnabled(true);
    enabled = true;
}

void TagWidget::disable()
{
    setEnabled(false);
    enabled = false;
}
