#ifndef FILESOURCE_H
#define FILESOURCE_H

#include <QIODevice>
#include <QFile>
#include <QByteArray>

class FileSource : public QIODevice
{
public:
    FileSource(QByteArray image,
               QString filename,
               QVector<QPair<QString,QString> > fields);
//virtual ~QIODevice ()
    virtual bool atEnd (); const
    virtual bool canReadLine ();
    virtual void close ();
    virtual bool isSequential ();
    virtual bool open ( OpenMode mode );
    virtual qint64 pos ();
    virtual bool reset ();
    virtual bool seek ( qint64 pos );
    virtual qint64 size ();
    virtual bool waitForBytesWritten ( int msecs );
    virtual bool waitForReadyRead ( int msecs );
    qint64 read ( char * data, qint64 maxSize );
    QByteArray read ( qint64 maxSize );
    QByteArray readAll ();

private:
    QByteArray header, footer;
    int curPos;
    QFile data;

#endif // FILESOURCE_H
