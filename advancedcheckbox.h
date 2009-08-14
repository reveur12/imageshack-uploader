#ifndef ADVANCEDCHECKBOX_H
#define ADVANCEDCHECKBOX_H

#include <QtGui/QWidget>

namespace Ui {
    class AdvancedCheckBox;
}

class AdvancedCheckBox : public QWidget {
    Q_OBJECT
public:
    AdvancedCheckBox(QWidget *parent = 0);
    ~AdvancedCheckBox();
    bool isChecked();
    void setText(QString);
    void setChecked(bool);
    void setEnabled(bool);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AdvancedCheckBox *m_ui;

private slots:
    void checkboxClicked(int);

signals:
    void stateChanged(int);
};

#endif // ADVANCEDCHECKBOX_H
