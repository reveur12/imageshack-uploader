/*
Copyright (c) 2009, ImageShack Corp.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the ImageShack nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <QFile>
#include <QPair>
#include <QImage>
#include <QColor>
#include <QDebug>
#include <QTemporaryFile>
#include <QApplication>
#include "videopreviewcreator.h"

VideoPreviewCreator::VideoPreviewCreator()
{
}

QByteArray VideoPreviewCreator::getImageData(fas_raw_image_type *image)
{
    QByteArray res;

    res.append(QString("P6\n%1 %2\n255\n").arg(image->width).arg(image->height));

    // Write pixel data
    for(int h=0; h<image->height; h++)
        for(int w=0; w<image->width*3; w++)
            res.append(*(image->data + h * image->bytes_per_line+w));

    return res;
}

static void ppm_save(fas_raw_image_type *image, char *filename)
{
    FILE *f;
    int i;

    if (image->color_space != FAS_RGB24) {
      return;
    }

    f=fopen(filename,"wb");
    fprintf(f,"P6\n%d %d\n%d\n", image->width, image->height, 255);

    for(i=0; i<image->height; i++) {
      fwrite(image->data + i * image->bytes_per_line, 1, image->width * 3, f);
    }

    fclose(f);
}

QVector<int> VideoPreviewCreator::getHistogram(QImage img)
{
    QVector<int> hist;
    hist.fill(0, 768);
    for(int i=0; i<img.width(); i++)
        for(int j=0; j<img.height(); j++)
        {
            QColor color;
            color.setRgb(img.pixel(i, j));
            hist[color.red()] += 1;
            hist[color.green()+256] += 1;
            hist[color.blue()+512] += 1;
        }
    return hist;
}

QVector<qint64> VideoPreviewCreator::getMedian(QVector<QPair<QVector<int>, int> > hists)
{
    QVector<qint64> median;
    median.fill(0, 768);
    for (int i=0; i<hists.size(); i++)
        for (int j=0; j<768; j++)
            median[j] += hists[i].first[j];
    for(int i=0; i<768; i++)
        median[i] /= hists.size();
    return median;
}

QPair<QVector<int>, int> VideoPreviewCreator::getClosest(QVector<QPair<QVector<int>, int> > hists, QVector<qint64> median)
{
    QVector<qint64> diffs;
    diffs.fill(0, hists.size());
    for (int i=0; i< hists.size(); i++)
    {
        QVector<int> hist = hists[i].first;

        qint64 sum = 0;
        for(int j=0; j<768; j++)
            sum += (median[j]-hist[j])*(median[j]-hist[j]);
        diffs[i] = sum;
    }
    qint64 min = Q_INT64_C(932838457459459);
    int min_n = diffs.size();
    for(int i=0; i<diffs.size(); i++)
        if (diffs[i]<min)
        {
            min = diffs[i];
            min_n = i;
        }
    return hists.at(min_n);
}

QImage VideoPreviewCreator::getPreview(QString filename)
{
    fas_error_type video_error;
    fas_context_ref_type context;
    fas_raw_image_type image_buffer;

    fas_initialize (FAS_FALSE, FAS_RGB24);

    video_error = fas_open_video (&context, filename.toLocal8Bit().data());
    if (video_error != FAS_SUCCESS)
    {
        qDebug() << "failed to open video file";
        return QImage();
    }

    QByteArray res;
    int counter = 0;
    QVector<QPair<QVector<int>, int> > hists;
    bool error = false;
    while (counter < 75)
    {
        if (FAS_SUCCESS != fas_get_frame (context, &image_buffer))
        {
            error = true;
            break;
        }
        QImage img;
        if (!img.loadFromData(getImageData(&image_buffer)))
        {
            error = true;
            break;
        }
        hists.append(qMakePair(getHistogram(img), counter));
        fas_free_frame (image_buffer);
        if (FAS_SUCCESS != fas_step_forward (context))
        {
            error = true;
            break;
        }
        counter++;
    }
    if (error) return QImage();

    QPair<QVector<int>, int> frame = getClosest(hists, getMedian(hists));

    fas_seek_to_frame(context, frame.second);
    fas_get_frame (context, &image_buffer);
    QImage img;
    img.loadFromData(getImageData(&image_buffer));
    fas_free_frame (image_buffer);

    return img;
}

