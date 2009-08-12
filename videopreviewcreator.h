#ifndef VIDEOPREVIEWCREATOR_H
#define VIDEOPREVIEWCREATOR_H

#include <QObject>
#include <QByteArray>
extern "C"
{
#include "ffmpeg_fas.h"
}

class VideoPreviewCreator : public QObject
{
public:
    VideoPreviewCreator();
    QByteArray getImageData(fas_raw_image_type *);
    QString getPreview(QString);
};

#endif // VIDEOPREVIEWCREATOR_H
