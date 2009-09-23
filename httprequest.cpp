#include "httprequest.h"
#include "filesource.h"
#include <QDomElement>
#include <QDomDocument>

HTTPRequest::HTTPRequest()
{

}

void HTTPRequest::put(QString url, QMap<QString, QString> fields)
{
}

bool HTTPRequest::putFile(QSharedPointer<Media> media, QMap<QString, QString> fields)
{
    if (inProgress) return false;
    failed = false;
    inProgress = true;
    QNetworkRequest req(VIDEO_UPLOAD_HOST + VIDEO_UPLOAD_PATH + '/start');
    reply = QSharedPointer(qnam.post(req, formStartPostData));
    reply.data()->connect(SIGNAL(finished()), this, SLOT(putFile2()));
    reply.data()->connect(SIGNAL(error(QNetworkReply::NetworkError)),
                          this, SLOT(fail(QNetworkReply::NetworkError)));
    return true;
}

void HTTPRequest::putFile2(QString uploadUrl)
{
    if (uploadUrl.isEmpty())
    {
        if (failed) return;
        QString data = reply.data()->readAll();
        QDomDocument xml;
        xml.setContent(data);
        QDomElement doc = xml.documentElement();
        if (doc.isNull())
        {
            fail("Server returned invalid response");
            return;
        }
        url = doc.attribute("putURL");
        if (url.isEmpty())
        {
            fail("Got not upload url from server");
            return;
        }
    }
    else { url = uploadUrl; }
    QNetworkRequest req(url);
    reply = QSharedPointer(qnam.head(req));
    reply.data()->connect(SIGNAL(finished()), this, SLOT(putFile3()));
    reply.data()->connect(SIGNAL(error(QNetworkReply::NetworkError)),
                          this, SLOT(fail(QNetworkReply::NetworkError)));
}

void HTTPRequest::putFile3()
{
    if (failed) return;
    qint64 doneSize = reply.data()->header(QNetworkRequest::ContentLengthHeader).toInt();
    FileSource *fs = new FileSource(media, fields);
    fs->seek(doneSize);
    QNetworkRequest req(url);
    QString header = QString::number(doneSize) + '-' + QString::number(fs->size());
    req.setRawHeader("Content-Range", header);
    reply = QSharedPointer(qnam.put(req, fs));
    reply.data()->connect(SIGNAL(finished()), this, SLOT(putFile4()));
    reply.data()->connect(SIGNAL(error(QNetworkReply::NetworkError)),
                          this, SLOT(fail(QNetworkReply::NetworkError)));
    reply.data()->connect(SIGNAL(uploadProgress(qint64, qint64)), this,
                          SLOT(putFileProgressReceiver(qint64, qint64)));
}

void HTTPRequest::putFileProgressReceiver(qint64 done, qint64 total)
{
    emit progress((doneSize+done)*100/(doneSize+total));
}

void HTTPRequest::putFile4()
{
    if (failed) return;    
    QString data = reply.data()->readAll();
    emit result(data);
    inProgress = false;
}

void HTTPRequest::fail(QString msg)
{
    if (failed) return;
    failed = true;
    inProgress = false;
    reply.data()->blockSignals(true);
    emit error(msg);
}

void HTTPRequest::fail(QNetworkReply::NetworkError code)
{
    fail("Error #" + QString::number(code));
}

void HTTPRequest::get(QString url, QMap<QString, QString> params)
{
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
    reply = QSharedPointer(qnam.get(req));
    reply.data()->connect(SIGNAL(finished()), this,
                 SLOT(getReceiver()));
    reply.data()->connect(SIGNAL(error(QNetworkReply::NetworkError)), this,
                 SLOT(fail(QNetworkReply::NetworkError)));
}

void HTTPRequest::getReceiver()
{
    if (failed) return;
    emit result(reply.data()->readAll());
    inProgress = false;
}

void HTTPRequest::post(QString url, QMap<QString, QString> fields)
{
    failed = false;
    inProgress = true;
    QByteArray data;
    foreach(QString key, fields.keys())
    {
        if (data.size() && !data.endsWith('&')) data.append('&');
        data.append(key + '=' + fields[key]);
    }
    QNetworkRequest req(url);
    reply = QSharedPointer(qnam.post(req, data));
    reply.data()->connect(SIGNAL(finished()), this,
                          SLOT(postReceiver()));
    reply.data()->connect(SIGNAL(error(QNetworkReply::NetworkError)), this,
                 SLOT(fail(QNetworkReply::NetworkError)));
}

void HTTPRequest::postReceiver()
{
    if (failed) return;
    emit result(reply.data()->readAll());
    inProgress = falsel;
}

void HTTPRequest::postFile(QSharedPointer<Media> media, QMap<QString, QString> fields)
{
    failed = false;
    inProgress = true;
    FileSource fs(media, fields);
    reply = QSharedPointer(qnam.post(req, fs));
}

void HTTPRequest::postFileReceiver()
{
    if (failed) return;
    emit result(reply.data()->readAll());
    inProgress = falsel;
}

void HTTPRequest::pause()
{
    reply.data()->blockSignals(true);
    reply.data()->abort();
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

void progressReceiver(qint64, qint64)
{
}

void statusReceiver(int)
{
}
