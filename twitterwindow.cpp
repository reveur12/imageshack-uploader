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


#include <QSettings>
#include <QMessageBox>
#include "twitterwindow.h"
#include "ui_twitterwindow.h"

TwitterWindow::TwitterWindow(QDialog *parent) :
    QDialog(parent),
    m_ui(new Ui::TwitterWindow)
{
    setParent(parent);
    m_ui->setupUi(this);
    twitter = QSharedPointer<TwitterClient>(new TwitterClient(parent));
    textChanged();
    QSettings sets;
    QString user = QByteArray::fromBase64(sets.value("twitteruser",
                                                  QVariant("")).toByteArray());
    QString pass = QByteArray::fromBase64(sets.value("twitterpass",
                                                  QVariant("")).toByteArray());
    m_ui->remember->setChecked(((!pass.isEmpty()) && (!user.isEmpty())));
    m_ui->username->setText(user);
    m_ui->password->setText(pass);

    connect(twitter.data(),
            SIGNAL(errorHappened()),
            this,
            SLOT(twitterError()));
}

TwitterWindow::~TwitterWindow()
{
    delete m_ui;
}

void TwitterWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        twitter.data()->bar.setWindowTitle(tr("Posting to twitter..."));
        break;
    default:
        break;
    }
}

void TwitterWindow::textChanged()
{
    int symbols = m_ui->text->toPlainText().length();
    m_ui->symbols->setText(tr("Symbols left: %1").arg(100-symbols));
}

void TwitterWindow::rememberChanged()
{
    // I dont remember what this function should do, so it is commented untill
    // i call back. Anyway everything works without it.
    /*
    QSettings sets;
    if (m_ui->remember->isChecked())
    {
        QByteArray user, pass;
        user.append(m_ui->username->text());
        pass.append(m_ui->password->text());
        sets.setValue("twitteruser", QVariant(user.toBase64()));
        sets.setValue("twitterpass", QVariant(pass.toBase64()));
    }
    else
    {
        sets.setValue("twitteruser", QVariant());
        sets.setValue("twitterpass", QVariant());
    }*/
}

void TwitterWindow::submit()
{
    rememberChanged();
    if (m_ui->username->text().isEmpty() || m_ui->password->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("You have not entered username or password"));
        return;
    }
    QSettings sets;
    if (m_ui->remember->isChecked())
    {
        QByteArray user, pass;
        user.append(m_ui->username->text());
        pass.append(m_ui->password->text());
        sets.setValue("twitteruser", QVariant(user.toBase64()));
        sets.setValue("twitterpass", QVariant(pass.toBase64()));
    }
    else
    {
        sets.setValue("twitteruser", QVariant());
        sets.setValue("twitterpass", QVariant());
    }
    QPoint barpos;
    barpos.setX(pos().x() + this->width()/2);
    barpos.setY(pos().y() + this->height()/2);
    if (gallery)
        twitter.data()->post(links, m_ui->text->toPlainText(),
                             m_ui->username->text(), m_ui->password->text(),
                             shortlink, true, barpos);
    else
        twitter.data()->post(link, m_ui->text->toPlainText(),
                             m_ui->username->text(), m_ui->password->text(),
                             true, barpos);
    hide();
}

void TwitterWindow::post(QString data)
{
    m_ui->text->clear();
    link = data;
    gallery = false;
    show();
}

void TwitterWindow::post(QStringList data, QString slink)
{
    if (data.size() == 1) return post(data.at(0));
    m_ui->text->clear();
    links = data;
    shortlink = slink;
    gallery = true;
    show();
}

void TwitterWindow::twitterError()
{
    show();
}
