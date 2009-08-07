#!/usr/bin/env python
#
# Build static list of ffmpeg libraries

import sys,re,os,platform
from subprocess import Popen, PIPE

LIBS=['libavcodec','libavformat','libswscale','libavutil']
LIBPATHS = ['/usr/lib','/usr/local/lib']
EXCLUDE_LIBS = ['dirac_decoder']



def main():

    if platform.system()!='Darwin':
        print " ".join(["-l%s" % l[3:] for l in LIBS])
        sys.exit(0);
    
    lib_paths = LIBPATHS
    for l in LIBS:
        o = Popen(["pkg-config", l, "--static", "--libs"], stdout=PIPE)\
            .communicate()[0]
        for lpath in re.findall(r'-L([^ ]+)', o):
            if lpath not in lib_paths:
                lib_paths = [lpath] + lib_paths
                
    lnames = []
    for l in LIBS:
        o = Popen(["pkg-config", l, "--static", "--libs-only-l"], stdout=PIPE)\
            .communicate()[0]
        for ln in re.findall(r'-l([^ ]+)', o):
            if ln not in lnames and ln not in EXCLUDE_LIBS:
                lnames.append(ln)

                
    res=[]
    for l in lnames:
        for lpath in lib_paths:
            p = "%s/lib%s.a" % (lpath,l)
            if os.path.exists(p):
                res.append(p)
                break
    print " ".join(res)
    
if __name__ == "__main__":
    main()
