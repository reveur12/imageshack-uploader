#ifndef PAUSEDIALOG_H
#define PAUSEDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class PauseDialog;
}

class PauseDialog : public QDialog {
    Q_OBJECT
public:
    PauseDialog(QWidget *parent = 0);
    ~PauseDialog();
    void accept();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PauseDialog *m_ui;
};

#endif // PAUSEDIALOG_H
