#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QMap>

class HTTPRequest : public QObject
{
public:
    HTTPRequest();
    void put(QString url, QMap<QString, QString> fields);
    void get(QString url, QMap<QString, QString> params);
    void postFile(QString url, QMap<QString, QString> fiels, QString filename);
    void pause();
    void resume();
    int progress();
    void connectProgress(const char* to, const char* func);
    void connectStatus(const char* to, const char* func);
    enum state { START, END, ERROR, RESUME, PAUSE };
private:
    QNetworkAccessManager qnam;
    QSharedPointer<QNetworkReply> reply;
private slots:
    void progressReceiver(qint64, qint64);
    void statusReceiver(int);
signals:
    void progress(int);
    void status(state);
};

#endif // HTTPREQUEST_H
