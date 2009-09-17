#include "httprequest.h"

HTTPRequest::HTTPRequest()
{

}

void HTTPRequest::put(QString url, QMap<QString, QString> fields)
{
}

void HTTPRequest::get(QString url, QMap<QString, QString> params)
{
}

void postFile(QString url, QMap<QString, QString> fiels, QString filename)
{
}

void pause()
{
}

void resume()
{
}

int progress()
{
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
