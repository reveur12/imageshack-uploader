#ifndef FILESOURCE_H
#define FILESOURCE_H

#include <QIODevice>
#include <QFile>
#include <QByteArray>
#include <QPair>
#include <QSharedPointer>
#include <QVector>
#include "media.h"

class FileSource : public QIODevice
{
    Q_OBJECT
public:
    FileSource(QSharedPointer<Media> media,
               QVector<QPair<QString,QString> > fields);
    bool atEnd () const;
    qint64 size () const;
    qint64 readData(char*, qint64);
    qint64 writeData ( const char * data, qint64 maxSize );
    qint64 bytesAvailable() const;
    bool isSequential() const;

private:
    QByteArray header, footer;
    int curPos;
    QFile *data;

};

#endif // FILESOURCE_H
