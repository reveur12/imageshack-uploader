# Requirements #

In order to build MSI you need to have following prerequisires meet

  * Compiled imageshack uploader (release mode), see BuildInstructions for how-to.
  * [Wix 3](http://wix.sourceforge.net/) binaries in your PATH
  * Installed Qt libraries (you needed them for building too)
  * FFMpeg libraries in your project directory:
    * avcodec.dll
    * avformat.dll
    * avutil.dll
    * swscale.dll

# Build imageshack uploader from sources #

See BuildInstructions document.

# Make MSI file #

Run `mingw32-make msi` or, if you are using Visual Studio's NMake, `nmake msi`.

All needed libraries will be copied to `dlls` directory automatically and ImageShackUploader.msi will appear in your project directory.