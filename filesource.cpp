#include "filesource.h"
#include "defines.h"
#include <QDebug>

FileSource::FileSource(QSharedPointer<Media> media,
                       QVector<QPair<QString,QString> > fields)
{
    QString filename = media.data()->filepath();
    QString mediaClass = media.data()->getClass();
    QString mediaType = media.data()->getType();
    QString host;
    if ((mediaClass == "image") || (mediaClass == "application"))
        host = "load"+QString::number((qrand()%9)+1)+"." + UPLOAD_HOSTNAME;
    else
        host = VIDEO_UPLOAD_HOSTNAME;
    QString boundary("UPLOADERBOUNDARY");
    QString nl = "\r\n";

    header.append("--"); header.append(boundary); header.append(nl);
    header.append("Content-Disposition: ");
    header.append(QString("form-data; name=\"%1\"; filename=\"%2\"")
                 .arg(QString("fileupload"), QString(filename.toUtf8())));
    header.append(nl);
    header.append(QString("Content-Type: %1/%2").arg(mediaClass, mediaType));
    header.append(nl);
    header.append(nl);
    footer.append(nl);

    QPair<QString, QString> field;
    foreach(field, fields)
    {
        footer.append("--"); footer.append(boundary); footer.append(nl);
        footer.append("Content-Disposition: ");
        footer.append(QString("form-data; name=\"%1\"").arg(field.first));
        footer.append(nl);
        footer.append(nl);
        footer.append(field.second); footer.append(nl);
    }

    footer.append("--");
    footer.append(boundary);
    footer.append(nl);

    data = QSharedPointer<QFile>(new QFile(filename));
    data->open(QIODevice::ReadOnly);
    curPos = 0;
}

bool FileSource::atEnd () const
{
    return curPos == size();
}

qint64 FileSource::size () const
{
    return (header.size() + data->size() + footer.size());
}
qint64 FileSource::readData(char* to, qint64 max)
{
    QByteArray buf;

    while ((curPos < header.size()) && buf.size() < max)
    {
        buf.append(header.at(curPos));
        curPos++;
    }
    while ((curPos < (header.size()+data->size())) && buf.size() < max)
    {
        // read data
        int lpos = curPos - header.size();
        data->seek(lpos);
        if ( (data->size() - lpos) < max )
        {
            buf.append(data->read(data->size()-lpos));
            curPos += data->size()-lpos;
        }
        else
        {
            int oldbufsize = buf.size();
            buf.append(data->read(max - oldbufsize));
            curPos += max - oldbufsize;
        }
    }
    int fullsize = header.size() + data->size() + footer.size();
    while ((curPos >= ( header.size()+data->size()) && (curPos < fullsize)) && buf.size() < max)
    {
        buf.append(footer.at(curPos-header.size()-data->size()));
        curPos++;
    }

    memcpy(to, buf.data(), buf.size());

    if (buf.size())
        return buf.size();
    else return -1;
}

qint64 FileSource::writeData ( const char *, qint64)
{
    return -1;
}

qint64 FileSource::bytesAvailable() const
{
    return size() + QIODevice::bytesAvailable() - curPos;
}

bool FileSource::isSequential() const
{
    return false;
}
