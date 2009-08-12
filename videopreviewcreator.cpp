#include <QFile>
#include <QDebug>
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


QByteArray VideoPreviewCreator::getPreview(QString filename)
{
  fas_error_type video_error;
  fas_context_ref_type context;
  fas_raw_image_type image_buffer;

  fas_initialize (FAS_FALSE, FAS_RGB24);

  video_error = fas_open_video (&context, (char*)filename.toStdString().c_str());
  if (video_error != FAS_SUCCESS)
    qDebug() << "failed to open";

  int counter = 0;
  while (fas_frame_available (context))
    {

      qDebug() << counter;
      if (FAS_SUCCESS != fas_get_frame (context, &image_buffer))
        qDebug() << "failed on rgb image";

      //char filename[50];

      //fprintf(stderr, "Writing %s (counter=%d frame_index=%d)\n", filename, counter, fas_get_frame_index(context));
      //ppm_save(&image_buffer, filename);
      //char filename[50];
      //sprintf(filename, "/home/a2k/tmp/ff/%04d.ppm", counter);
      //ppm_save(&image_buffer, filename);
      QByteArray res = getImageData(&image_buffer);
      /*
      QFile f("/home/a2k/tmp/ff/"+QString::number(counter)+".ppm");
      f.open(QFile::WriteOnly);
      f.write(res);
      f.close();*/

      fas_free_frame (image_buffer);

      video_error = fas_step_forward (context);
      counter++;
      return res;
    }

}

