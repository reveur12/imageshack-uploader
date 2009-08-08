#!/usr/bin/env python
#
# Add frameworks to app

import sys,re,os,platform
from subprocess import Popen, PIPE, call

QT_LIB_PATH='/opt/local/libexec/qt4-mac/lib'
FRAMEWORKS=['QtXml', 'QtGui', 'QtNetwork', 'QtCore']

def main():

    app_path = sys.argv[1]
    dmg_path = sys.argv[2]

    app_frameworks_path = "%s/Contents/Frameworks" % app_path
    if not os.path.exists(app_frameworks_path):
        os.mkdir(app_frameworks_path)

    # copy frameworks
    for f in FRAMEWORKS:
        print "Copying %s" % f
        if call(["cp", "-r", "%s/%s.framework" % (QT_LIB_PATH,f), app_frameworks_path]) != 0:
            print "Error copying frameworks"
            sys.exit(1)

    # get full framework paths
    fpaths = {}
    for f in FRAMEWORKS:
        o = Popen(["otool", "-L", "%s/%s.framework/%s" % (QT_LIB_PATH,f,f)], stdout=PIPE)\
            .communicate()[0]
        m = re.match(r'^\s*([^\s]+)\s', o.split('\n')[1])
        fpaths[f]=m.group(1)[len(QT_LIB_PATH)+1:]

    # adjust frameforks IDs
    for f in FRAMEWORKS:
        fp = fpaths[f]
        cmd = ["install_name_tool", "-id", \
               "@executable_path/../Frameworks/%s" % (fp), \
               "%s/Contents/Frameworks/%s" % (app_path,fp)\
               ]
        if call(cmd) != 0:
            print "Error executing: %s" % cmd
            sys.exit(1)

    # adjust framework refs from executable
    for f in FRAMEWORKS:
        fp = fpaths[f]
        cmd = ["install_name_tool", "-change", \
               "%s/%s" % (QT_LIB_PATH,fp), \
               "@executable_path/../Frameworks/%s" % (fp), \
               "%s/Contents/MacOS/%s" % (app_path,app_path[:-4])
               ]
        print cmd
        if call(cmd) != 0:
            print "Error executing: %s" % cmd
            sys.exit(1)





if __name__ == "__main__":
    main()


#cp -R /path/to/Qt/lib/QtCore.framework
#         plugandpaint.app/Contents/Frameworks
#cp -R /path/to/Qt/lib/QtGui.framework
#        plugandpaint.app/Contents/Frameworks

# rm -f $DMG
#hdiutil create -fs HFS+ -srcfolder ImageShackUploader.app  -volname ImageShackUploader $DMG
