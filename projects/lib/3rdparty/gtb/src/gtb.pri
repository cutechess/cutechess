DEFINES += Z_PREFIX
CONFIG(release, debug|release):DEFINES += NDEBUG

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += $$PWD/gtb-probe.c \
    $$PWD/gtb-dec.c \
    $$PWD/gtb-att.c \
    $$PWD/sysport/sysport.c \
    $$PWD/compression/wrap.c \
    $$PWD/compression/huffman/hzip.c \
    $$PWD/compression/liblzf/lzf_c.c \
    $$PWD/compression/liblzf/lzf_d.c \
    $$PWD/compression/zlib/zcompress.c \
    $$PWD/compression/zlib/uncompr.c \
    $$PWD/compression/zlib/inflate.c \
    $$PWD/compression/zlib/deflate.c \
    $$PWD/compression/zlib/adler32.c \
    $$PWD/compression/zlib/crc32.c \
    $$PWD/compression/zlib/infback.c \
    $$PWD/compression/zlib/inffast.c \
    $$PWD/compression/zlib/inftrees.c \
    $$PWD/compression/zlib/trees.c \
    $$PWD/compression/zlib/zutil.c \
    $$PWD/compression/lzma/LzmaEnc.c \
    $$PWD/compression/lzma/LzmaDec.c \
    $$PWD/compression/lzma/Alloc.c \
    $$PWD/compression/lzma/LzFind.c \
    $$PWD/compression/lzma/Lzma86Enc.c \
    $$PWD/compression/lzma/Lzma86Dec.c \
    $$PWD/compression/lzma/Bra86.c
HEADERS += $$PWD/gtb-att.h \
    $$PWD/gtb-dec.h \
    $$PWD/gtb-probe.h \
    $$PWD/gtb-types.h \
    $$PWD/sysport/sysport.h \
    $$PWD/compression/wrap.h \
    $$PWD/compression/huffman/hzip.h \
    $$PWD/compression/liblzf/lzf.h \
    $$PWD/compression/liblzf/lzfP.h \
    $$PWD/compression/zlib/crc32.h \
    $$PWD/compression/zlib/deflate.h \
    $$PWD/compression/zlib/inffast.h \
    $$PWD/compression/zlib/inffixed.h \
    $$PWD/compression/zlib/inflate.h \
    $$PWD/compression/zlib/inftrees.h \
    $$PWD/compression/zlib/trees.h \
    $$PWD/compression/zlib/zconf.h \
    $$PWD/compression/zlib/zconf.in.h \
    $$PWD/compression/zlib/zlib.h \
    $$PWD/compression/zlib/zutil.h \
    $$PWD/compression/lzma/Alloc.h \
    $$PWD/compression/lzma/Bra.h \
    $$PWD/compression/lzma/LzFind.h \
    $$PWD/compression/lzma/LzFindMt.h \
    $$PWD/compression/lzma/LzHash.h \
    $$PWD/compression/lzma/Lzma86Dec.h \
    $$PWD/compression/lzma/Lzma86Enc.h \
    $$PWD/compression/lzma/LzmaDec.h \
    $$PWD/compression/lzma/LzmaEnc.h \
    $$PWD/compression/lzma/Types.h
