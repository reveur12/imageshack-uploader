#include "httprequest.h"
#include "filesource.h"
#include <QDomElement>
#include <QDomDocument>
#include <QDebug>

HTTPRequest::HTTPRequest()
{
    failed = false;
    inProgress = false;
}

void HTTPRequest::put(QString url, QVector<QPair<QString, QString> > fields)
{
}

void HTTPRequest::connectReply(const char* func)
{
    connect(reply, SIGNAL(finished()), this, func);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(fail(QNetworkReply::NetworkError)));
}

QByteArray HTTPRequest::formStartPostData(QSharedPointer<Media> media, QVector<QPair<QString, QString> > fields)
{
    return QByteArray("key=0369BIKP4dc39f5f287a680e200ac4c2bc821f3a");
}

bool HTTPRequest::putFile(QSharedPointer<Media> media, QVector<QPair<QString, QString> > fields)
{
    if (inProgress) return false;
    failed = false;
    inProgress = true;
    emit progress(0);
    this->media = media;
    this->fields = fields;
    QNetworkRequest req("http://" + QString(CHUNKED_VIDEO_UPLOAD_HOSTNAME) + CHUNKED_VIDEO_UPLOAD_PATH + "/start");
    qDebug() << "putFile making request...";

    reply = qnam.post(req, formStartPostData(media, fields));
    //reply = r;
    //connect(reply, SIGNAL(finished()), this, SLOT(putFile2()));
    //connect(reply, SIGNAL(finished()), this, SLOT(putFile2()));
    connectReply(SLOT(putFile2()));
    return true;
}

void HTTPRequest::putFile2(QString uploadUrl, QString lenUrl)
{
    qDebug() << "putFile2 got response";
    emit progress(1);
    if (uploadUrl.isEmpty())
    {
        if (failed) return;
        QString data = reply->readAll();
        QDomDocument xml;
        xml.setContent(data);
        QDomElement doc = xml.documentElement();
        if (doc.isNull())
        {
            fail("Server returned invalid response");
            return;
        }
        url = doc.attribute("putURL");
        getlenurl = doc.attribute("getlengthURL");
        if (url.isEmpty() || getlenurl.isEmpty())
        {
            fail("Got no upload url from server");
            return;
        }

    }
    else { url = uploadUrl; getlenurl = lenUrl; }
    QNetworkRequest req(getlenurl);
    reply = qnam.get(req);
    connectReply(SLOT(putFile3()));
}

void HTTPRequest::putFile3()
{
    emit progress(2);
    qDebug() << "putFile3 got response";
    if (failed) return;
    bool isok = true;
    doneSize = reply->readAll().toInt(&isok);
    if (!isok) doneSize = 0;
    //qint64 doneSize = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
    qDebug() << "opening filesource";
    //FileSource *fs = new FileSource(media, fields);
    QFile *fs = new QFile(media.data()->filepath());
    fs->open(QFile::ReadOnly);
    fs->seek(doneSize);
    qDebug() << "created filesource";
    //fs->open(FileSource::ReadOnly);
    //fs->seek(doneSize);
    qDebug() << "opened filesource";
    qDebug() << "puting to url" << url;
    QNetworkRequest req(url);
    QByteArray header;
    header.append("bytes " + QString::number(doneSize) + '-' + QString::number(fs->size()) + '/' + QString::number(fs->size()));
    req.setRawHeader(QByteArray("Content-Range"), header);
    reply = qnam.put(req, fs);
    connectReply(SLOT(putFile4()));
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this,
            SLOT(putFileProgressReceiver(qint64, qint64)));
    qDebug() << "putFile3 requested";
}

void HTTPRequest::putFileProgressReceiver(qint64 done, qint64 total)
{
    emit progress(2 + (doneSize+done)*98/(doneSize+total));
}

void HTTPRequest::putFile4()
{
    qDebug() << "putFile4 got response";
    if (failed) return;
    QString data = reply->readAll();
    qDebug() << data;
    emit result(data);
    inProgress = false;
}

void HTTPRequest::fail(QString msg)
{
    qDebug() << msg;
    if (failed) return;
    failed = true;
    inProgress = false;
    reply->blockSignals(true);
    emit error(msg);
}

void HTTPRequest::fail(QNetworkReply::NetworkError code)
{
    qDebug() << "GOT ERROR!!!";
    qDebug() << reply->readAll();
    fail("Error #" + QString::number(code));
}

void HTTPRequest::get(QString url, QVector<QPair<QString, QString> > params)
{/*
    failed = false;
    inProgress = true;
    if (!url.endsWith('?')) url += '?';
    foreach(QString key, params.keys())
    {
        if (!url.endsWith('?') && !url.endsWith('&'))
            url += '&';
        url += QUrl::toPercentEncoding(key) + '=' + QUrl::toPercentEncoding(params[key]);
    }
    QNetworkRequest req(url);
    reply = QSharedPointer<QNetworkReply>(qnam.get(req));
    reply->connect(SIGNAL(finished()), this,
                 SLOT(getReceiver()));
    reply->connect(SIGNAL(error(QNetworkReply::NetworkError)), this,
                 SLOT(fail(QNetworkReply::NetworkError)));*/
}

void HTTPRequest::getReceiver()
{
    if (failed) return;
    emit result(reply->readAll());
    inProgress = false;
}

void HTTPRequest::post(QString url, QVector<QPair<QString, QString> > fields)
{/*
    failed = false;
    inProgress = true;
    QByteArray data;
    foreach(QString key, fields.keys())
    {
        if (data.size() && !data.endsWith('&')) data.append('&');
        data.append(key + '=' + fields[key]);
    }
    QNetworkRequest req(url);
    reply = QSharedPointer<QNetworkReply>(qnam.post(req, data));
    reply->connect(SIGNAL(finished()), this,
                          SLOT(postReceiver()));
    reply->connect(SIGNAL(error(QNetworkReply::NetworkError)), this,
                 SLOT(fail(QNetworkReply::NetworkError)));*/
}

void HTTPRequest::postReceiver()
{
    if (failed) return;
    emit result(reply->readAll());
    inProgress = false;
}

void HTTPRequest::postFile(QSharedPointer<Media> media, QVector<QPair<QString, QString> > fields)
{
    failed = false;
    inProgress = true;
    FileSource fs(media, fields);

    //reply = QSharedPointer<QNetworkReply>(qnam.post(req, fs));
}

void HTTPRequest::postFileReceiver()
{
    if (failed) return;
    emit result(reply->readAll());
    inProgress = false;
}

void HTTPRequest::pause()
{
    reply->blockSignals(true);
    reply->abort();
}

void HTTPRequest::resume()
{
    if (media.isNull() || fields.isEmpty()) return;
    this->putFile2(url);
}

void connectProgress(const char* to, const char* func)
{
}

void connectStatus(const char* to, const char* func)
{
}
