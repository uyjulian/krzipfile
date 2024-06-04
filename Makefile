
MINIZIP_SOURCES += external/minizip_lib/bzip2/blocksort.c external/minizip_lib/bzip2/bzlib.c external/minizip_lib/bzip2/compress.c external/minizip_lib/bzip2/crctable.c external/minizip_lib/bzip2/decompress.c external/minizip_lib/bzip2/huffman.c external/minizip_lib/bzip2/randtable.c external/minizip_lib/liblzma/check/check.c external/minizip_lib/liblzma/check/crc32_fast.c external/minizip_lib/liblzma/check/crc32_table.c external/minizip_lib/liblzma/common/alone_decoder.c external/minizip_lib/liblzma/common/alone_encoder.c external/minizip_lib/liblzma/common/common.c external/minizip_lib/liblzma/common/filter_encoder.c external/minizip_lib/liblzma/lz/lz_decoder.c external/minizip_lib/liblzma/lz/lz_encoder.c external/minizip_lib/liblzma/lz/lz_encoder_mf.c external/minizip_lib/liblzma/lzma/fastpos_table.c external/minizip_lib/liblzma/lzma/lzma_decoder.c external/minizip_lib/liblzma/lzma/lzma_encoder.c external/minizip_lib/liblzma/lzma/lzma_encoder_optimum_fast.c external/minizip_lib/liblzma/lzma/lzma_encoder_optimum_normal.c external/minizip_lib/liblzma/lzma/lzma_encoder_presets.c external/minizip_lib/liblzma/rangecoder/price_table.c external/minizip/mz_compat.c external/minizip/mz_crypt.c external/minizip/mz_os.c external/minizip/mz_os_win32.c external/minizip/mz_strm.c external/minizip/mz_strm_buf.c external/minizip/mz_strm_bzip.c external/minizip/mz_strm_lzma.c external/minizip/mz_strm_mem.c external/minizip/mz_strm_os_win32.c external/minizip/mz_strm_pkcrypt.c external/minizip/mz_strm_split.c external/minizip/mz_strm_wzaes.c external/minizip/mz_strm_zlib.c external/minizip/mz_zip.c external/minizip/mz_zip_rw.c
ZLIB_SOURCES += external/zlib-ng/adler32.c external/zlib-ng/adler32_fold.c external/zlib-ng/chunkset.c external/zlib-ng/compare256.c external/zlib-ng/compress.c external/zlib-ng/cpu_features.c external/zlib-ng/crc32_braid.c external/zlib-ng/crc32_braid_comb.c external/zlib-ng/crc32_fold.c external/zlib-ng/deflate.c external/zlib-ng/deflate_fast.c external/zlib-ng/deflate_huff.c external/zlib-ng/deflate_medium.c external/zlib-ng/deflate_quick.c external/zlib-ng/deflate_rle.c external/zlib-ng/deflate_slow.c external/zlib-ng/deflate_stored.c external/zlib-ng/functable.c external/zlib-ng/infback.c external/zlib-ng/inflate.c external/zlib-ng/inftrees.c external/zlib-ng/insert_string.c external/zlib-ng/insert_string_roll.c external/zlib-ng/slide_hash.c external/zlib-ng/trees.c external/zlib-ng/uncompr.c external/zlib-ng/zutil.c

SOURCES += storage.cpp CharacterSet.cpp
SOURCES += $(MINIZIP_SOURCES) $(ZLIB_SOURCES)

LDLIBS += -lole32 -luuid

INCFLAGS += -Iexternal/zlib-ng -Iexternal/minizip -Iexternal/minizip_lib/bzip2 -Iexternal/minizip_lib/liblzma -Iexternal/minizip_lib/liblzma/api -Iexternal/minizip_lib/liblzma/check -Iexternal/minizip_lib/liblzma/common -Iexternal/minizip_lib/liblzma/lz -Iexternal/minizip_lib/liblzma/lzma -Iexternal/minizip_lib/liblzma/rangecoder

CFLAGS += -DHAVE_ATTRIBUTE_ALIGNED -DHAVE_PKCRYPT -DHAVE_WZAES -DMZ_ZIP_NO_SIGNING -D__USE_FILE_OFFSET64 -D__USE_LARGEFILE64 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_STDINT_H -DHAVE_INTTYPES_H -DHAVE_ZLIB -DHAVE_BZIP2 -DBZ_NO_STDIO -DHAVE_LZMA -DLZMA_API_STATIC -DHAVE_CONFIG_H -DHAVE_LIMITS_H -DHAVE_STRING_H -DHAVE_STRINGS_H -DHAVE_MEMORY_H -DHAVE_STDBOOL_H -DHAVE_IMMINTRIN_H -DSYMBOLIC_LINK_FLAG_DIRECTORY=0x1

PROJECT_BASENAME = krzipfile

RC_LEGALCOPYRIGHT ?= Copyright (C) 2022-2022 Julian Uy; See details of license at license.txt, or the source code location.

include external/ncbind/Rules.lib.make
