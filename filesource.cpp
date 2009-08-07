#include "filesource.h"
#include "defines.h"
#include <QHttp>
#include <QHttpRequestHeader>
#include <QDebug>

FileSource::FileSource(QByteArray image,
                       QString filename,
                       QVector<QPair<QString,QString> > fields)
{
    QString host;
    if ((mediaClass == "image") || (mediaClass == "application"))
        host = "load"+QString::number((qrand()%9)+1)+"." + UPLOAD_HOSTNAME;
    else
        host = VIDEO_UPLOAD_HOSTNAME;
    QString boundary("UPLOADERBOUNDARY");
    QString nl = "\r\n";

    QHttpRequestHeader header("POST", UPLOAD_PATH, 1, 1);
    header.addValue("Content-type",
                    "multipart/form-data, boundary=\"" + boundary+"\"");
    header.addValue("Cache-Control", "no-cache");
    header.addValue("Host", host);
    header.addValue("Accept","*/*");

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


    data.open(filename, QIODevice::ReadOnly);
    pos = 0;
}

virtual bool FileSource::atEnd ()
{
    return curPos == size();
}

virtual bool FileSource::canReadLine ()
{
    return pos<size();
}

virtual void FileSource::close ()
{
    data.close();
    return True;
}

virtual bool FileSource::isSequential ()
{
    return False;
}

virtual bool FileSource::open ( OpenMode mode )
{
    return true;
}

virtual qint64 FileSource::pos ()
{
    return curPos;
}

virtual bool FileSource::reset ()
{
    curPos = 0;
    return True;
}

virtual bool FileSource::seek ( qint64 pos )
{
    curPos = pos;
    return true;
}

virtual qint64 FileSource::size ()
{
    return header.size() + data.size() + footer.size();
}

virtual bool FileSource::waitForBytesWritten ( int msecs )
{
    qDebug() << "waitForBytesWritten called";
}

virtual bool FileSource::waitForReadyRead ( int msecs )
{
    qDebug() << "waitForReadyRead called";
}

qint64 FileSource::read ( char * data, qint64 maxSize )
{
    qDebug() << "WRONG READ CALLED!!!";
}

QByteArray FileSource::read ( qint64 maxSize )
{
    QByteArray res;
    while (maxSize)
    {
        if (curPos < header.size())
        {
            res.append(header.at(curPos));
            maxsize -= 1;
            pos++;
        }
        else if (curPos < (header.size() + data.size()))
        {
            data.seek(pos - header.size());
            QByteArray data = data.read(maxSize);
            maxSize -= data.size();
            curPos += data.size();
        }
        else if (curPos < size())
        {
            res.append(footer.at(curPos));
            pos++;
        }
        else return res;
    }
    return res;
}

QByteArray FileSource::readAll ()
{
    qDebug() << "READ ALL CALLED!!!!";
}
