#include "httprequest.h"
#include "filesource.h"
#include <QApplication>
#include <QDomElement>
#include <QDomDocument>
#include <QDebug>

HTTPRequest::HTTPRequest()
{
    failed = false;
    inProgress = false;
}

void HTTPRequest::uploadFile(QSharedPointer<Media> media, QString cookie, QString username, QString password)
{
    if (media.data()->getClass() == "video")
    {
        qDebug() << media.data()->uploadURL << media.data()->sizeURL;
        if (!media.data()->uploadURL.isEmpty() && !media.data()->sizeURL.isEmpty())
        {
            inProgress = true;
            failed = false;
            this->media = media;
            this->putFile2(media.data()->uploadURL, media.data()->sizeURL);
        }
        else
            this->putFile(media, cookie, username, password);
    }
    else
    {
        this->postFile(media, cookie, username, password);
    }
}

QByteArray HTTPRequest::userAgent()
{
    QByteArray userAgent;
    userAgent += "ImageShack Uploader";
    userAgent += " " + QApplication::applicationVersion() + " ";

#ifdef Q_OS_WIN
    userAgent += "(Windows)";
#endif
#ifdef Q_WS_X11
#ifndef Q_OS_MACX
    userAgent += "(Unix)";
#endif
#endif
#ifdef Q_OS_MACX
    userAgent += "(MacOS)";
#endif
    return userAgent;
}

QVector<QPair<QString, QString> > HTTPRequest::formDataFields(QSharedPointer<Media> media, QString cookie, QString username, QString password)
{
    QVector<QPair<QString,QString> > fields;
    QString tags = media.data()->getAllTags();
    bool isPublic = !(media.data()->getPrivate());
    fields << qMakePair(QString("public"), QString(isPublic?"yes":"no"));
    if (!cookie.isEmpty()) fields << qMakePair(QString("cookie"), cookie);
    if (!username.isEmpty()) fields << qMakePair(QString("username"), username);
    if (!password.isEmpty()) fields << qMakePair(QString("password"), password);
    if (!tags.isEmpty()) fields << qMakePair(QString("tags"), tags);
    if ((media.data()->getClass() == "image") || (media.data()->getClass() == "application"))
        {
        if (!media.data()->getResize().isNull())
        {
            fields << qMakePair(QString("optimage"), QString("1"));
            fields << qMakePair(QString("optsize"), media.data()->getResize());
        }
        if (media.data()->getRemoveSize()) fields << qMakePair(QString("rembar"), QString("1"));
    }
    fields << qMakePair(QString("key"), QString(DEVELOPER_KEY));
    return fields;
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

QByteArray HTTPRequest::formStartPostData(QSharedPointer<Media> media, QString cookie, QString username, QString password)
{
    QByteArray res;
    QVector<QPair<QString, QString> > fields = formDataFields(media, cookie, username, password);
    for(int i=0; i<fields.size(); i++)
    {
        if (res.size() && !res.endsWith('&')) res.append('&');
        res.append(fields.at(i).first + "=" + fields.at(i).second);
    }
    return res;
}

bool HTTPRequest::putFile(QSharedPointer<Media> media, QString cookie, QString username, QString password)
{
    if (inProgress) return false;
    failed = false;
    inProgress = true;
    emit progress(0);
    this->media = media;
    if (!media.data()->sizeURL.isEmpty() && !media.data()->uploadURL.isEmpty())
    {
        putFile2(media.data()->uploadURL, media.data()->sizeURL);
        return true;
    }
    QNetworkRequest req("http://" + QString(CHUNKED_VIDEO_UPLOAD_HOSTNAME) + CHUNKED_VIDEO_UPLOAD_PATH + "/start");
    qDebug() << formStartPostData(media, cookie, username, password);
    reply = qnam.post(req, formStartPostData(media, cookie, username, password));
    //connectReply(SLOT(putFile2()));
    connect(reply, SIGNAL(finished()), this, SLOT(putFile2()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(specialFail(QNetworkReply::NetworkError)));
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

        media.data()->sizeURL = getlenurl;
        media.data()->uploadURL = url;

    }
    else { url = uploadUrl; getlenurl = lenUrl; }
    qDebug() << "resuming upload...";

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
    QFile *fs = new QFile(media.data()->filepath());
    fs->open(QFile::ReadOnly);
    fs->seek(doneSize);
    QNetworkRequest req(url);
    QByteArray header;
    header.append("bytes " + QString::number(doneSize) + '-' + QString::number(fs->size()) + '/' + QString::number(fs->size()));
    req.setRawHeader(QByteArray("Content-Range"), header);
    req.setRawHeader(QByteArray("Content-Length"), QByteArray().append(QString::number(fs->size() - doneSize)));
    req.setRawHeader(QByteArray("Content-Type"), QByteArray("application/octet-stream"));
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

void HTTPRequest::postFileProgressReceiver(qint64 done, qint64 total)
{
    emit progress(done*100/total);
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

void HTTPRequest::specialFail(QNetworkReply::NetworkError code)
{
    qDebug() << "Special fail";
    qDebug() << reply->readAll();
    reply->blockSignals(true);
    this->postFile(media, cookie, username, password);
}


void HTTPRequest::get(QString url, QVector<QPair<QString, QString> > params)
{
    failed = false;
    inProgress = true;
    if (!url.endsWith('?')) url += '?';
    for (int i=0; i<params.size(); i++)
    {
        QString key = params.at(i).first;
        QString arg = params.at(i).second;
        if (!url.endsWith('?') && !url.endsWith('&'))
            url += '&';
        url += QUrl::toPercentEncoding(key) + '=' + QUrl::toPercentEncoding(arg);
    }
    QNetworkRequest req(url);
    reply = qnam.get(req);
    connectReply(SLOT(getReceiver()));
}

void HTTPRequest::getReceiver()
{
    if (failed) return;
    emit result(reply->readAll());
    inProgress = false;
}

void HTTPRequest::post(QString url, QVector<QPair<QString, QString> > fields)
{
    failed = false;
    inProgress = true;
    QByteArray data;
    for(int i=0; i<fields.size(); i++)
    {
        QString key = fields.at(i).first;
        QString value = fields.at(i).second;
        if (data.size() && !data.endsWith('&')) data.append('&');
        data.append(key + '=' + value);
    }
    QNetworkRequest req(url);
    reply = qnam.post(req, data);
    connectReply(SLOT(postReceiver()));
}

void HTTPRequest::postReceiver()
{
    if (failed) return;
    emit result(reply->readAll());
    inProgress = false;
}

QPair<QString, QString> HTTPRequest::getUploadHost(QSharedPointer<Media> media)
{
    QString host, path;
    if ((media.data()->getClass() == "image") || (media.data()->getClass() == "application"))
    {
        host = "load"+QString::number((qrand()%9)+1)+"." + UPLOAD_HOSTNAME;
        path = UPLOAD_PATH;
    } else
    {
        host = VIDEO_UPLOAD_HOSTNAME;
        path = VIDEO_UPLOAD_PATH;
    }
    return qMakePair(host, path);
}

void HTTPRequest::postFile(QSharedPointer<Media> media, QString cookie, QString username, QString password)
{
    failed = false;
    inProgress = true;

    QString host, path;
    QPair<QString, QString> destination = getUploadHost(media);
    host = destination.first;
    path = destination.second;

    QString boundary(BOUNDARY);

    data = QSharedPointer<FileSource>(new FileSource(media, fields));
    data.data()->open(QIODevice::ReadOnly);

    QNetworkRequest req(QUrl("http://" + QString(host) + path));
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  "multipart/form-data, boundary=" + boundary);

    req.setRawHeader("User-Agent", userAgent());

    reply = qnam.post(req, data.data());

    connectReply(SLOT(postFileReceiver()));
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)),
            this, SLOT(postFileProgressReceiver(qint64,qint64)));
}

void HTTPRequest::postFileReceiver()
{
    if (failed) return;
    QString data = reply->readAll();
    qDebug() << data;
    emit result(data);
    inProgress = false;
}

void HTTPRequest::stop()
{
    pause();
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

void HTTPRequest::connectResult(QObject* obj, const char* func)
{
    connect(this, SIGNAL(result(QString)), obj, func);
}

void HTTPRequest::connectProgress(QObject* obj, const char* func)
{
    connect(this, SIGNAL(progress(int)), obj, func);
}

void HTTPRequest::connectError(QObject* obj, const char* func)
{
    connect(this, SIGNAL(error(QString)), obj, func);
}

QString HTTPRequest::errorString()
{
    if (reply == NULL) return QString("No error");
    return reply->errorString();
}
