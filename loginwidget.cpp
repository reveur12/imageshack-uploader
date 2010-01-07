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


#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QMessageBox>
#include <QMovie>
#include <QSettings>

LoginWidget::LoginWidget(QWidget *parent) :
    QStackedWidget(parent),
    m_ui(new Ui::LoginWidget)
{
    m_ui->setupUi(this);

    QMovie *progress = new QMovie(":/images/images/loading.gif");
    progress->start();
    m_ui->progressLabel->setMovie(progress);

    QSettings sets;
    QByteArray lbytes, pbytes;
    lbytes = sets.value("login", QVariant("")).toByteArray();
    pbytes = sets.value("password", QVariant("")).toByteArray();
    QString login = QByteArray::fromBase64(lbytes);
    QString pass = QByteArray::fromBase64(pbytes);
    m_ui->login->setText(login);
    m_ui->password->setText(pass);
    m_ui->rememberPassword->setChecked(sets.value("rememberPass", QVariant(true)).toBool());
    if ((!login.isEmpty()) && (!pass.isEmpty()))
        m_ui->loginButton->click();

    connect(&request,
            SIGNAL(success(QString, QString)),
            this,
            SLOT(successReceiver(QString, QString)));
    connect(&request,
            SIGNAL(failed()),
            this,
            SLOT(failReceiver()));
    connect(&request,
            SIGNAL(wrongPassword()),
            this,
            SLOT(wrongPasswordReceiver()));
    //setLoggedIn(false);
    wasLoggedIn = false;
}

LoginWidget::~LoginWidget()
{
    delete m_ui;
}

void LoginWidget::changeEvent(QEvent *e)
{
    QStackedWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        m_ui->username->setText(tr("You are logged in as %1").arg(username));
        break;
    default:
        break;
    }
}

bool LoginWidget::isLoggedIn()
{
    return loggedIn;
}

QString LoginWidget::getCookie()
{
    return cookie;
}

void LoginWidget::logIn()
{
    cookie.clear();
    QString login = m_ui->login->text();
    QString pass = m_ui->password->text();

    QSettings sets;
    if (m_ui->rememberPassword->isChecked())
    {
        QByteArray lbytes, pbytes;
        lbytes.append(m_ui->login->text());
        pbytes.append(m_ui->password->text());
        sets.setValue("login", QVariant(lbytes.toBase64()));
        sets.setValue("password", QVariant(pbytes.toBase64()));
    }
    else
    {
        sets.setValue("login", QVariant(""));
        sets.setValue("password", QVariant(""));
    }
    sets.setValue("rememberPass", QVariant(m_ui->rememberPassword->isChecked()));

    setCurrentIndex(1);
    request.login(login, pass);
}

void LoginWidget::logOut()
{
    cookie.clear();
    setLoggedIn(false);
    setCurrentIndex(0);
    emit loginStatus(1);
}

void LoginWidget::successReceiver(QString value, QString name)
{
    cookie = value;
    username = name;
    setCurrentIndex(2);
    m_ui->username->setText(tr("You are logged in as %1").arg(name));
    setLoggedIn(true);
    emit loginStatus(0);
}

void LoginWidget::failReceiver()
{
    setCurrentIndex(0);
    QString error = request.http.errorString();
    QString text = tr("Could not connect to ImageShack server.");
    if (error != "Unknown error")
        text += "\n" + error;
    QMessageBox::critical(this, tr("Authentication failed"), text);
    setLoggedIn(false);
    emit loginStatus(1);
}

void LoginWidget::wrongPasswordReceiver()
{
    setCurrentIndex(0);
    QMessageBox::critical(this, tr("Authentication failed"),
                       tr("Wrong credentials."));
    setLoggedIn(false);
    emit loginStatus(1);
}

void LoginWidget::cancel()
{
    request.abort();
    setCurrentIndex(0);
    cookie.clear();
    setLoggedIn(false);
    emit loginStatus(1);
}

void LoginWidget::setLoggedIn(bool value)
{
    QSettings sets;
    sets.setValue("loggedin", QVariant(value));
    loggedIn = value;
    if (value) wasLoggedIn = true;
}

void LoginWidget::rememberChange()
{
    if (!m_ui->rememberPassword->isChecked())
    {
        QSettings sets;
        QByteArray lbytes, pbytes;
        sets.setValue("password", QVariant(""));
        if(wasLoggedIn)
        {
            m_ui->username->clear();
            m_ui->password->clear();
        }
    }
}
