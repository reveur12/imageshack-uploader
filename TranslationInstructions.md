# Translating #
ImageShack Uploader uses Qt's translation system.

To translate application to your language you need to get one of `*`.ts files used in uploader. Each `*`.ts files contains translation to one language.

Base application language is English, so the best choise is to get en\_US.ts file. You can download it from out Mercurial repository (it lays in "translations" source directory).

To work with `*`.ts file you can use any text editor, but the best choise is Qt's Linguist. You can download it here: http://code.google.com/p/qtlinguistdownload/downloads/list

Open downloaded `*`.ts file with Qt Linguist and translate it to your language.

**Important: change LANGUAGE\_NAME in MainWindow to your language name.** Otherwise your translation language will not be detected.

Sends us back translated file so we could include it in next release.

This is how it looks like:

![http://img8.imageshack.us/img8/4817/scrt.png](http://img8.imageshack.us/img8/4817/scrt.png)

# Testing your translation #
First you need to compile your `*`.ts file to `*`.qm. Use file->Release in Qt Linguist menu for this.

Uploader looks for translations in this directories:
  * Mac OS: APP\_DIRETORY/Contents/Resource
  * Windows: INSTALLTION\_DIRECTORY/translations
  * Linux: /usr/share/imageshack-uploader/translations
Copy your translation file to translations directory and your language should be available in Uploader's options dialog.

# Sending your translations to us #
[Open](http://code.google.com/p/imageshack-uploader/issues/entry) a new issue and attach both ts and qm files you created. It is very important to attach ts file, because we will not add your translation to future release without it. And qm file is needed for QA to check your translations.