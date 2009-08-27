#include <QSettings>
#include "pausedialog.h"
#include "ui_pausedialog.h"

PauseDialog::PauseDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PauseDialog)
{
    m_ui->setupUi(this);
}

PauseDialog::~PauseDialog()
{
    delete m_ui;
}

void PauseDialog::changeEvent(QEvent *e)
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

void PauseDialog::accept()
{
    if (m_ui->hide->isChecked())
    {
        QSettings sets;
        sets.setValue("hidePauseWarning", QVariant(true));
    }
    QDialog::accept();
}
