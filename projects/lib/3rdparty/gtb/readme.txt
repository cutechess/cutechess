                 Gaviota Tablebases Probing Code API
                Copyright (c) 2010-2011 Miguel A. Ballicora
-----------------------------------------------------------------------------

This software provides C code to probe the Gaviota Endgame Tablebases.
It is released under then X11 ("MIT") license (see license.txt).

This API (Application Programming Interface) is designed to be as portable 
as possible. Functions could be called from Linux or Windows. 
Most likely it will work in other operating systems but that has not been 
tested. This API is a beta version and as such, it is not guaranteed any 
type of backward compatibility or to remain untouched, at least until 
version 1.0 is released. 

A very small set of tablebase files is included in this distribution 
for testing purposes (only 3 pieces). They are compressed with four
different compression schemes. For a more complete set, please download 
Gaviota from

http://sites.google.com/site/gaviotachessengine/

and generate the 4 and 5 piece tablebases. Instructions how to generate
and compressed them are in the website. More information can always be found:

http://sites.google.com/site/gaviotachessengine/Home/endgame-tablebases-1

Alternatively, already compressed tablebase files (ready to go!) can be 
downloaded from

http://www.olympuschess.com/egtb/gaviota/ (Many thanks to Josh Shriver)

"tbprobe" is distributed here as a tablebase probing example. The current API
is relatively "low level" to optimize performance. We hope the small program 
tbprobe is self explanatory. A thorough and detailed documentation may be 
released later. However, it seems that everybody who attempted to implement
this probing code was successful relatively easy.

We plan to support an interface with a FEN notation; thus, it is expected 
that some other functions maybe added to this API.

Four different types of compression are included. It is possible that in the
future some other compression schemes could be provided, but only if they
represent a serious improvement in speed or memory size. To maximize
backward compatibility between versions of programs and TBs, it is strongly
recommended that engine developers always support at least scheme 4 (tb_CP4), 
which is considered the default at this point. For that reason, it is 
suggested that testers always have a set of TBs compressed with scheme 4.

This API is designed to be multithreading friendly. Regions where different 
threads could access data from this API were protected with a mutex to avoid
problems.

-------------------------- How to use this API ------------------------------

To include this code in any engine or GUI, the following files should be
compiled and linked:

gtb-probe.c
gtb-dec.c
gtb-att.c
sysport/sysport.c  
compression/wrap.c
compression/huffman/hzip.c 
compression/liblzf/lzf_c.c
compression/liblzf/lzf_d.c
compression/zlib/zcompress.c
compression/zlib/uncompr.c
compression/zlib/inflate.c
compression/zlib/deflate.c
compression/zlib/adler32.c   
compression/zlib/crc32.c    
compression/zlib/infback.c  
compression/zlib/inffast.c  
compression/zlib/inftrees.c  
compression/zlib/trees.c     
compression/zlib/zutil.c
compression/lzma/LzmaEnc.c 
compression/lzma/LzmaDec.c 
compression/lzma/Alloc.c 
compression/lzma/LzFind.c 
compression/lzma/Lzma86Enc.c 
compression/lzma/Lzma86Dec.c 
compression/lzma/Bra86.c

The following files will be "included" 
gtb-probe.h  
gtb-dec.h  
gtb-att.h
gtb-types.h

plus all the *.h files in the folders, so set the proper -I flags:
sysport/
compression/
compression/huffman/
compression/liblzf/
compression/zlib/
compression/lzma/

The following libraries should be linked in Linux
-lpthread
-lm

In Windows, the appropriate MT (multithreaded library should be linked too)

These switches should be set in the compiler
-D NDEBUG
-D Z_PREFIX

The first one removes the assert code, and the second
one makes sure that there is no collision between some names in the 
zlib library and names in other compression libraries.

-------------------------- COMPILATION EXAMPLE ------------------------------

The file compile.sh is an example of how tbprobe can be
compiled in Linux using gcc.

Rakefile.rb is the ruby version of Makefile. You have to install 'rake'
to execute it. This is what I use but you don't have to. It is provided
out of laziness. I should probably remove it.

------------------ COMPILING A STATIC LIBRARY (optional) --------------------

Aaron Becker wrote a Makefile to compile a static library --> libgtb.a
I just applied this modification from his fork.
For now, this for Linux only. Type 'make' to compile it.
Some people may find this approach more convenient since the library
has to be compiled only once. Of course, this library needs to be included
at linking time, when you compile your own program

---------------------------- For UCI Authors --------------------------------

Generally, UCI (Universal Chess Interface) GUIs use standard labels for
Tablebase paths and cache sizes. For instance, NalimovPath and NalimovCache
are used for the Nalimov tablebases. Therefore, engine authors are strongly 
encouraged (Please!) to follow a common standard to simplify the life of GUI 
developers and users. For that reason, it is suggested to implement as 
parameters: GaviotaTbPath and GaviotaTbCache in their communication with a
UCI graphical user interface.

-----------------------------------------------------------------------------

Good luck with the tablebases!

Miguel

*****************************************************************************
