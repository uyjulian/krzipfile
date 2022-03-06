
MINIZIP_SOURCES += external/minizip/lib/bzip2/blocksort.c external/minizip/lib/bzip2/bzlib.c external/minizip/lib/bzip2/compress.c external/minizip/lib/bzip2/crctable.c external/minizip/lib/bzip2/decompress.c external/minizip/lib/bzip2/huffman.c external/minizip/lib/bzip2/randtable.c external/minizip/lib/liblzma/check/check.c external/minizip/lib/liblzma/check/crc32_fast.c external/minizip/lib/liblzma/check/crc32_table.c external/minizip/lib/liblzma/common/alone_decoder.c external/minizip/lib/liblzma/common/alone_encoder.c external/minizip/lib/liblzma/common/common.c external/minizip/lib/liblzma/common/filter_encoder.c external/minizip/lib/liblzma/lz/lz_decoder.c external/minizip/lib/liblzma/lz/lz_encoder.c external/minizip/lib/liblzma/lz/lz_encoder_mf.c external/minizip/lib/liblzma/lzma/fastpos_table.c external/minizip/lib/liblzma/lzma/lzma_decoder.c external/minizip/lib/liblzma/lzma/lzma_encoder.c external/minizip/lib/liblzma/lzma/lzma_encoder_optimum_fast.c external/minizip/lib/liblzma/lzma/lzma_encoder_optimum_normal.c external/minizip/lib/liblzma/lzma/lzma_encoder_presets.c external/minizip/lib/liblzma/rangecoder/price_table.c external/minizip/mz_compat.c external/minizip/mz_crypt.c external/minizip/mz_crypt_win32.c external/minizip/mz_os.c external/minizip/mz_os_win32.c external/minizip/mz_strm.c external/minizip/mz_strm_buf.c external/minizip/mz_strm_bzip.c external/minizip/mz_strm_lzma.c external/minizip/mz_strm_mem.c external/minizip/mz_strm_os_win32.c external/minizip/mz_strm_pkcrypt.c external/minizip/mz_strm_split.c external/minizip/mz_strm_wzaes.c external/minizip/mz_strm_zlib.c external/minizip/mz_zip.c external/minizip/mz_zip_rw.c
ZLIB_SOURCES += external/zlib/adler32.c external/zlib/compress.c external/zlib/crc32.c external/zlib/deflate.c external/zlib/gzclose.c external/zlib/gzlib.c external/zlib/gzread.c external/zlib/gzwrite.c external/zlib/infback.c external/zlib/inffast.c external/zlib/inflate.c external/zlib/inftrees.c external/zlib/trees.c external/zlib/uncompr.c external/zlib/zutil.c

SOURCES += storage.cpp CharacterSet.cpp
SOURCES += $(MINIZIP_SOURCES) $(ZLIB_SOURCES)

LDLIBS += -lole32 -luuid

INCFLAGS += -Iexternal/zlib -Iexternal/minizip -Iexternal/minizip/lib/bzip2 -Iexternal/minizip/lib/liblzma -Iexternal/minizip/lib/liblzma/api -Iexternal/minizip/lib/liblzma/check -Iexternal/minizip/lib/liblzma/common -Iexternal/minizip/lib/liblzma/lz -Iexternal/minizip/lib/liblzma/lzma -Iexternal/minizip/lib/liblzma/rangecoder

CFLAGS += -DHAVE_PKCRYPT -DHAVE_WZAES -DMZ_ZIP_NO_SIGNING -D__USE_FILE_OFFSET64 -D__USE_LARGEFILE64 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_STDINT_H -DHAVE_INTTYPES_H -DHAVE_ZLIB -DHAVE_BZIP2 -DBZ_NO_STDIO -DHAVE_LZMA -DLZMA_API_STATIC -DHAVE_CONFIG_H -DHAVE_LIMITS_H -DHAVE_STRING_H -DHAVE_STRINGS_H -DHAVE_MEMORY_H -DHAVE_STDBOOL_H -DHAVE_IMMINTRIN_H -DSYMBOLIC_LINK_FLAG_DIRECTORY=0x1

PROJECT_BASENAME = krzipfile

RC_LEGALCOPYRIGHT ?= Copyright (C) 2022-2022 Julian Uy; See details of license at license.txt, or the source code location.

include external/ncbind/Rules.lib.make
