# Common part #
ImageShack Uploader uses portable `qmake` utility to build on various platforms. Most platforms, where Qt and FFmpeg are available, can be used to build from source.

There are only two dependencies:
  1. [Qt](http://www.qtsoftware.com/) v4.5 or newer (modules core, gui and xml)
  1. [ffmpeg](http://ffmpeg.org/) v0.5 or newer

ImageShack Uploader uses ImageShack and YFrog [APIs](http://code.google.com/p/imageshackapi/), so a [developer key](http://code.google.com/p/imageshackapi/wiki/DeveloperKey) is required to build. It should be specified in envirement variable IMAGESHACK\_DEVELOPER\_KEY. If this variable is empty, you will get an error on qmake step.

The easiest way to build this sources is using Qt SQK which is available for free on Trolltech website.

# Building on Unix #
  1. Install Qt 4.5 packages. For Ubuntu they are libqt4-gui, libqt4-core, libqt4-xml and qt developer tools.
  1. Install ffmpeg and it's developer package (headers).
  1. IMAGESHACK\_DEVELOPER\_KEY="YOURdeveloperKEY" qmake
  1. make

You can also build packages for common Linux distributions using `make deb` or `make rpm` or `make packages` to build both.

For other distros there is make install target.

# Building on Windows #
  1. Install [Mingw](http://www.mingw.org/) or [Microsoft Visual C++ 2008 Express Edition](http://www.microsoft.com/express/download/#webInstall) (free download from Microsoft).
  1. Get [Qt SDK](http://www.qtsoftware.com/downloads)
  1. Download or compile ffmpeg. You need following DLLs:
    * avformat.dll
    * avcodec.dll
    * swscale.dll
    * avutil.dll.
> > Copy them to source root directory. You can grab compiled DLLs at [http://ffmpeg.arrozcru.org/](http://ffmpeg.arrozcru.org/). Look there for ffmpeg Win32 shared libraries. You need ffmpeg headers as well. Download ffmpeg at [http://ffmpeg.org/download.html](http://ffmpeg.org/download.html) and copy
    * libavformat
    * libavutil
    * libavcodec
    * libswscale
> > to the source root directory recursive. In fact you need only header .h files.
  1. Add mingw and Qt's binaries to system search path. It should look like this: %SystemRoot%\system32;%SystemRoot%;C:\Qt\2009.03\mingw\bin;C:\Qt\2009.03\mingw\mingw32\bin;C:\Qt\2009.03\qt\bin;C:\Qt\2009.03\qt\qmake; - change "C:\Qt\2009.03" to your Qt SDK installation directory.
  1. Set enviroment variable IMAGESHACK\_DEVELOPER\_KEY to your developer key.
  1. Run `qmake CONFIG+=release` in project directory to generate Makefile.
  1. Run `mingw32-make` or `nmake` to compile project.
  1. Use "Dependency Walker" to find out what libraries are requires to run and copy them to build directory


# Building on Mac #

You need to have [Xcode Tools](http://developer.apple.com/technology/tools.html) installed. Due to a bug (see [issue #14](http://code.google.com/p/imageshack-uploader/issues/detail?id=14) for details) in  [MacPorts](http://www.macports.org/) version of QT at this moment you need to use [Qt SDK distributions (DMG)](http://www.qtsoftware.com/downloads).

To perform build, execute the following (assuming _bash_ shell):
```
IMAGESHACK_DEVELOPER_KEY="YOURdeveloperKEY" qmake imageshack.pro -spec macx-g++
make
```

To build distributable DMG image, use `make dmg` command.