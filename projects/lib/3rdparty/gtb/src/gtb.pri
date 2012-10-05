DEFINES += Z_PREFIX
CONFIG(release, debug|release):DEFINES += NDEBUG

DEPENDPATH += $$PWD
SOURCES += gtb-probe.c \
    gtb-dec.c \
    gtb-att.c \
    sysport/sysport.c \
    compression/wrap.c \
    compression/huffman/hzip.c \
    compression/liblzf/lzf_c.c \
    compression/liblzf/lzf_d.c \
    compression/zlib/zcompress.c \
    compression/zlib/uncompr.c \
    compression/zlib/inflate.c \
    compression/zlib/deflate.c \
    compression/zlib/adler32.c \
    compression/zlib/crc32.c \
    compression/zlib/infback.c \
    compression/zlib/inffast.c \
    compression/zlib/inftrees.c \
    compression/zlib/trees.c \
    compression/zlib/zutil.c \
    compression/lzma/LzmaEnc.c \
    compression/lzma/LzmaDec.c \
    compression/lzma/Alloc.c \
    compression/lzma/LzFind.c \
    compression/lzma/Lzma86Enc.c \
    compression/lzma/Lzma86Dec.c \
    compression/lzma/Bra86.c
HEADERS += gtb-att.h \
    gtb-dec.h \
    gtb-probe.h \
    gtb-types.h \
    sysport/sysport.h \
    compression/wrap.h \
    compression/huffman/hzip.h \
    compression/liblzf/lzf.h \
    compression/liblzf/lzfP.h \
    compression/zlib/crc32.h \
    compression/zlib/deflate.h \
    compression/zlib/inffast.h \
    compression/zlib/inffixed.h \
    compression/zlib/inflate.h \
    compression/zlib/inftrees.h \
    compression/zlib/trees.h \
    compression/zlib/zconf.h \
    compression/zlib/czonf.in.h \
    compression/zlib/zlib.h \
    compression/zlib/zutil.h \
    compression/lzma/Alloc.h \
    compression/lzma/Bra.h \
    compression/lzma/LzFind.h \
    compression/lzma/LzFindMt.h \
    compression/lzma/LzHash.h \
    compression/lzma/Lzma86Dec.h \
    compression/lzma/Lzma86Enc.h \
    compression/lzma/LzmaDec.h \
    compression/lzma/LzmaEnc.h \
    compression/lzma/Types.h
