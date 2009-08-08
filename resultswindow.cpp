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


#include "resultswindow.h"
#include "ui_resultswindow.h"
#include <QDebug>
#include <QPair>

ResultsWindow::ResultsWindow(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ResultsWindow)
{
    m_ui->setupUi(this);
    current = 0;
}

ResultsWindow::~ResultsWindow()
{
    delete m_ui;
}

void ResultsWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ResultsWindow::setResults(QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > > res)
{
    results = res;
    current = 0;
    setIndex(0);

    m_ui->all1->setTitle(tr("Direct links"));
    m_ui->all2->setTitle(tr("Short links"));
    m_ui->all3->setTitle(tr("Hotlinks for web"));
    m_ui->all4->setTitle(tr("Thumbnails for web"));
    m_ui->all5->setTitle(tr("Thumbnails for forums 1"));
    m_ui->all6->setTitle(tr("Thumbnails for forums 2"));
    m_ui->all7->setTitle(tr("Hotlinks for forums 1"));
    m_ui->all8->setTitle(tr("Hotlinks for forums 2"));

    QStringList box1, box2, box3, box4, box5, box6, box7, box8;
    QPair<QSharedPointer<Media>,QStringList> element;
    for (int i=0; i<results.data()->size(); i++)
    {
        element = results.data()->at(i);
        box1.append(element.second.at(0));
        if ( (element.first.data()->getClass() == "image")
            || (element.first.data()->getClass() == "application") )
        {
            box2.append(element.second.at(4));
            box3.append(element.second.at(1));
            box4.append(element.second.at(6));
            box5.append(element.second.at(7));
            box6.append(element.second.at(8));
            box7.append(element.second.at(2));
            box8.append(element.second.at(3));
        }
        else
        {
            box4.append(element.second.at(1));
            box5.append(element.second.at(2));
            box6.append(element.second.at(3));
        }
    }
    m_ui->all1->setText(box1.join("\n"));
    m_ui->all2->setText(box2.join("\n"));
    m_ui->all3->setText(box3.join("<br>\n"));
    m_ui->all4->setText(box4.join("<br>\n"));
    m_ui->all5->setText(box5.join("\n"));
    m_ui->all6->setText(box6.join("\n"));
    m_ui->all7->setText(box7.join("\n"));
    m_ui->all8->setText(box8.join("\n"));

}

void ResultsWindow::setIndex(int index)
{
    m_ui->thumbnail->setPixmap(QPixmap::fromImage(results.data()->at(index).first.data()->thumbnail()));
    m_ui->directLink->setText(results.data()->at(index).second.at(0));
    if ( (results.data()->at(index).first.data()->getClass() == "image")
        || (results.data()->at(index).first.data()->getClass() == "application") )
    {
        m_ui->directLink->setTitle(tr("Direct link"));
        m_ui->shortLink->setTitle(tr("Short link"));
        m_ui->hotlink->setTitle(tr("Hotlink for web"));
        m_ui->thumbnailLink->setTitle(tr("Thumbnail for web"));
        m_ui->forumThumbnail->setTitle(tr("Thumbnail for forums 1"));
        m_ui->forumThumbnail2->setTitle(tr("Thumbnail for forums 2"));
        m_ui->forumHotlink->setTitle(tr("Hotlink for forums 1"));
        m_ui->forumHotlink2->setTitle(tr("Hotlink for forums 2"));
        m_ui->directLink->setText(results.data()->at(index).second.at(0));
        m_ui->shortLink->setText(results.data()->at(index).second.at(4));
        m_ui->hotlink->setText(results.data()->at(index).second.at(1));
        m_ui->thumbnailLink->setText(results.data()->at(index).second.at(6));
        m_ui->forumThumbnail->setText(results.data()->at(index).second.at(7));
        m_ui->forumThumbnail2->setText(results.data()->at(index).second.at(8));
        m_ui->forumHotlink->setText(results.data()->at(index).second.at(2));
        m_ui->forumHotlink2->setText(results.data()->at(index).second.at(3));

        m_ui->hotlink->setEnabled(true);
        m_ui->forumHotlink->setEnabled(true);
        m_ui->forumHotlink2->setEnabled(true);
    }
    else if (results.data()->at(index).first.data()->getClass() == "video")
    {
        m_ui->directLink->setTitle(tr("Direct link"));
        m_ui->shortLink->setTitle(tr("Embedded video"));
        m_ui->hotlink->setTitle(tr("Hotlink for web"));
        m_ui->thumbnailLink->setTitle(tr("Thumbnail for web"));
        m_ui->forumThumbnail->setTitle(tr("Thumbnail for forums 1"));
        m_ui->forumThumbnail2->setTitle(tr("Thumbnail for forums 2"));
        m_ui->forumHotlink->setTitle(tr("Hotlink for forums 1"));
        m_ui->forumHotlink2->setTitle(tr("Hotlink for forums 2"));


        m_ui->directLink->setText(results.data()->at(index).second.at(0));
        m_ui->shortLink->setText(results.data()->at(index).second.at(4));
        m_ui->hotlink->setText(results.data()->at(index).second.at(2));
        m_ui->thumbnailLink->setText(results.data()->at(index).second.at(1));
        m_ui->forumThumbnail->setText(results.data()->at(index).second.at(2));
        m_ui->forumThumbnail2->setText(results.data()->at(index).second.at(3));
        m_ui->hotlink->setText("");
        m_ui->forumHotlink->setText("");
        m_ui->forumHotlink2->setText("");

        m_ui->hotlink->setEnabled(false);
        m_ui->forumHotlink->setEnabled(false);
        m_ui->forumHotlink2->setEnabled(false);        
    }
}

void ResultsWindow::prev()
{
    if (current>0) setIndex(--current);
}

void ResultsWindow::next()
{
    if (current<(results.data()->count()-1)) setIndex(++current);
}

void ResultsWindow::done()
{
    m_ui->thumbnail->setText("Thumbnail");
    results = QSharedPointer<QVector<QPair<QSharedPointer<Media>,QStringList> > >();
    reject();
}

void ResultsWindow::tweet()
{
    qDebug() << "twitting" << results.data()->at(current).second.at(0);
    twitter.post(results.data()->at(current).second.at(0));
}

void ResultsWindow::galleryCreate()
{
    QStringList urls;
    for(int i=0; i<results.data()->size(); i++)
    {
        urls.append(results.data()->at(i).second.at(0));
    }
    twitter.post(urls, results.data()->at(0).second.at(4)+"x");
}
