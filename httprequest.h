#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "media.h"
#include "defines.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QMap>

class HTTPRequest : public QObject
{
    Q_OBJECT
public:
    HTTPRequest();
    void put(QString url, QVector<QPair<QString, QString> > fields);
    void get(QString url, QVector<QPair<QString, QString> > params);
    void post(QString url, QVector<QPair<QString, QString> > fields);
    void postFile(QSharedPointer<Media> media, QVector<QPair<QString, QString> > fields);
    bool putFile(QSharedPointer<Media> media, QString cookie = "", QString username = "", QString password = "");
    void pause();
    void resume();
    int progress();
    void connectProgress(QObject* obj, const char* func);
    void connectResult(QObject* obj, const char* func);
    void connectError(QObject* obj, const char* func);
    enum state { START, END, ERROR, RESUME, PAUSE };
    QByteArray userAgent();
private:
    QNetworkAccessManager qnam;
    QNetworkReply* reply;
    bool inProgress;
    bool failed;
    void connectReply(const char*);
    qint64 doneSize;
    QString url, getlenurl;
    QSharedPointer<Media> media;
    QVector<QPair<QString, QString> > fields;
    QVector<QPair<QString, QString> > formDataFields(QSharedPointer<Media> media, QString cookie = "", QString username = "", QString password = "");
    QByteArray formStartPostData(QSharedPointer<Media> media, QString cookie, QString username, QString password);


    void getReceiver();
    void postReceiver();
    void postFileReceiver();


private slots:
    void putFile2(QString uploadUrl = "", QString lenUrl = "");
    void putFile3();
    void putFile4();
    void fail(QString msg);
    void fail(QNetworkReply::NetworkError code);
    void putFileProgressReceiver(qint64 done, qint64 total);
    //void progressReceiver(qint64, qint64);
    //void statusReceiver(int);
signals:
    void progress(int);
    void status(state);
    void result(QString);
    void error(QString);
};

#endif // HTTPREQUEST_H
