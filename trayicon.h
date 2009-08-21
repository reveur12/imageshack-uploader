#ifndef TRAYICON_H
#define TRAYICON_H

#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QRect>

class TrayIcon : public QObject
{
    Q_OBJECT
public:
    TrayIcon(QMainWindow *w = 0);
    void show();
    void hide();
    bool isHidden();

private:
    QSystemTrayIcon icon;
    QMenu menu;
    QMainWindow *parent;
    bool hidden;
    QRect lastPos;

public slots:
    void quit();
    void showUploader();
    void addFile();
    void iconClicked(QSystemTrayIcon::ActivationReason);
signals:
    void addFiles(QStringList);
};

#endif // TRAYICON_H
