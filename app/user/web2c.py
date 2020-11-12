#! /bin/env python3

import os
import glob
import binascii
import gzip
import re

try:
    io = __import__("io").BytesIO
except:
    io = __import__("StringIO").StringIO

def gen(dat, fn):
    try:
        s = ','.join(["0x%02x" % c for c in dat])
    except:
        s = ','.join(["0x"+binascii.hexlify(c) for c in dat])

    s = re.sub("((?:0x.+?,){16})", "\\1\n", s)

    fn = re.sub(r"[^\w]", "_", fn)
    return ("const unsigned char %s[0x%x] = {\n%s,0};\n\n" % (fn, len(dat)+1, s))
    #print("const unsigned char %s[0x%x] = {\n%s};" % (fn, len(dat), s))


outCfile = open("web_data.c","w")
outHfile = open("web_data.h","w")

outHfile.write("#ifndef __WEB_DATA_H__\n")
outHfile.write("#define __WEB_DATA_H__\n")

for fn in glob.glob('web/*.html'):
    dat=open(fn, 'rb').read();
    #ss=gen(open(fn, 'rb').read(), fn)
    try:
        s = ','.join(["0x%02x" % c for c in dat])
    except:
        s = ','.join(["0x"+binascii.hexlify(c) for c in dat])

    s = re.sub("((?:0x.+?,){16})", "\\1\n", s)

    fn = re.sub(r"[^\w]", "_", fn)

    outCfile.write("const unsigned char %s[0x%x] = {\n%s,0};\n\n" % (fn, len(dat)+1, s))
    outHfile.write("extern const unsigned char %s[0x%x];\n" % (fn, len(dat)+1))

outHfile.write("#endif\n")
outCfile.close()
outHfile.close()
