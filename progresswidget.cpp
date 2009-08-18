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


#include "progresswidget.h"
#include "ui_progresswidget.h"
#include <QPlastiqueStyle>
#include <QMessageBox>
#include <QMainWindow>
#include <QDebug>

ProgressWidget::ProgressWidget(QWidget *parent) :
    QStackedWidget(parent),
    m_ui(new Ui::ProgressWidget)
{
    setParent(parent);
    m_ui->setupUi(this);
#ifdef Q_OS_WIN
    m_ui->progressBar->setStyle(new QPlastiqueStyle());
    m_ui->progressBar_2->setStyle(new QPlastiqueStyle());
    m_ui->progressBar_3->setStyle(new QPlastiqueStyle());
#endif
    QFont font = QApplication::font();
    setFont(font);
#ifndef Q_OS_MACX
// On mac this font is small, but it is too big on windows default style
// and linux's Ozone (most popular Qt style).
    font.setPointSize(8);
    m_ui->ETA->setFont(font);
#endif
    paused = false;
}

ProgressWidget::~ProgressWidget()
{
    delete m_ui;
}

void ProgressWidget::changeEvent(QEvent *e)
{
    QStackedWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ProgressWidget::uploadClicked()
{
    emit upload();
}

void ProgressWidget::cancelClicked()
{
    emit cancel();
    paused = false;
    m_ui->pause->setText(tr("Pause"));
    m_ui->pause->setIcon(QIcon(":/images/images/pause.png"));
}

void ProgressWidget::setProgress(int total, int current)
{
    if (current == 100)
    {
        m_ui->progressBar_2->setFormat(tr("Waiting for links..."));
    }
    else
    {
        m_ui->progressBar_2->setFormat(QApplication::translate("ProgressWidget",
                                                               "Current: %p%",
                                                               0,
                                                               QApplication::UnicodeUTF8));
    }
    m_ui->progressBar->setValue(total);
    m_ui->progressBar_2->setValue(current);
}

void ProgressWidget::setMediaLoadProgress(int current, int total)
{
    qDebug() << "progress widget got values" << current << total;
    int tmp = current<total?current+1:total;
    QString format = tr("Loading files: %1 of %2").arg(tmp).arg(total);
    m_ui->progressBar_3->setFormat(format);
    m_ui->progressBar_3->setMaximum(total);
    m_ui->progressBar_3->setValue(current);
}

void ProgressWidget::setUploadEnabled(bool value)
{
    m_ui->upload->setEnabled(value);
}

void ProgressWidget::updateETA(int secs)
{
    if (secs == -100)
    {
        m_ui->ETA->setText(tr("Time left:")+" "+ tr("unknown..."));
        return;
    }
    int hours = secs/60/60;
    int minutes = (secs - hours*60*60)/60;
    int seconds = secs - (hours*60*60) - minutes*60;
    if (seconds>30) minutes++;
    if (minutes>59) { hours++; minutes -= 60; }
    QString text = tr("Time left:");
    if (minutes < 0) minutes = 0;
    if (hours < 0) hours = 0;
    if (seconds < 0) seconds = 0;
    if (hours) text.append(tr(" %n hrs.", 0, hours));
    if (minutes) text.append(tr(" %n min.", 0, minutes));
    //if (seconds) text.append(tr(" %n sec.", 0, seconds));
    if(!hours && !minutes && seconds) text.append(tr(" less than 1 min."));
    if(!hours && !minutes && !seconds) text.append(tr(" almost done..."));
    m_ui->ETA->setText(text);
}

void ProgressWidget::setCurrentIndex(int idx)
{
    QStackedWidget::setCurrentIndex(idx);
    if (idx == 1)
    {
        paused = false;
        m_ui->pause->setText(tr("Pause"));
        m_ui->pause->setIcon(QIcon(":/images/images/pause.png"));
        m_ui->ETA->setText(tr("Time left: estimating..."));
    }
}

void ProgressWidget::pauseClicked()
{
    if (!paused)
    {
        QMessageBox msg;
        msg.addButton(tr("Yes"), QMessageBox::YesRole);
        msg.addButton(tr("No"), QMessageBox::NoRole);
        msg.setText(tr("Pause will stop current file upload. On resume it will start uploading from begining. Are you sure want to pause?"));
        msg.setWindowTitle(tr("Pause"));
        msg.exec();
        if (msg.buttonRole(msg.clickedButton()) != QMessageBox::YesRole)
            return;
        m_ui->pause->setText(tr("Resume"));
        m_ui->pause->setIcon(QIcon(":/images/images/resume.png"));
        paused = true;
        emit pause(true);
    }
    else
    {
        emit pause(false);
        paused = false;
        m_ui->pause->setText(tr("Pause"));
        m_ui->pause->setIcon(QIcon(":/images/images/pause.png"));
    }
}
