#include <QDebug>
#include "advancedcheckbox.h"
#include "ui_advancedcheckbox.h"

AdvancedCheckBox::AdvancedCheckBox(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::AdvancedCheckBox)
{
    setParent(parent);
    m_ui->setupUi(this);
    //m_ui->label->setText(tr("Remove size/resolution bar from thumbnail"));
    setFont(QApplication::font());
}

AdvancedCheckBox::~AdvancedCheckBox()
{
    delete m_ui;
}

void AdvancedCheckBox::changeEvent(QEvent *e)
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

bool AdvancedCheckBox::isChecked()
{
    return m_ui->checkBox->isChecked();
}

void AdvancedCheckBox::setText(QString text)
{
    m_ui->label->setText(text);
}

void AdvancedCheckBox::setChecked(bool st)
{
    m_ui->checkBox->setChecked(st);
}

void AdvancedCheckBox::checkboxClicked(int st)
{
    emit stateChanged(st);
}

void AdvancedCheckBox::setEnabled(bool st)
{
    qDebug() << "ACB setEnabled(" << st<< ")";
    QWidget::setEnabled(st);
}
