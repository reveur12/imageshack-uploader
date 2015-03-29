This document lists all changes in official releases of ImageshackUploader

## Version 2.2 ##

This is a maintenance release.

### User-visible ###
  * New languages support  (user-contributed):
    * Arabic
    * Latvian
    * Polish
    * Thai
    * Turkish
    * Chinease
    * Italian
    * Hungarian.
  * If upload is paused or connection was lost, upload could be resumed without need to start over from the beginning of the file
  * Proxy support (SOCKS5 and HTTP Proxy).
  * Support for drag-and-drop of folders, not just individual files.
  * Under MacOS when right click on image or video files in Finder, ImageshackUploader is listed in list of offered applications in _"Open With"_ menu.
  * Various minor cosmetic, performance and reliability improvements

### For developers ###
  * Using [new gallery API](http://code.google.com/p/imageshackapi/wiki/ImageshackGalleryAPI), gallery is automatically created when batch of files is uploaded.
  * Could be built using Qt 4.6
  * Using [new video upload API](http://code.google.com/p/imageshackapi/wiki/ChunkedVideoUploadApiI) (if fails, falls back to old).