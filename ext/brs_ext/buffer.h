// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_BUFFER_H)
#define BRS_EXT_BUFFER_H

#include "ruby.h"

#define BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_COMPRESSOR (1 << 18)      // 256 KB
#define BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_COMPRESSOR (1 << 16) // 64 KB

#define BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_DECOMPRESSOR (1 << 16)      // 64 KB
#define BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_DECOMPRESSOR (1 << 18) // 256 KB

void brs_ext_buffer_exports(VALUE root_module);

#endif // BRS_EXT_BUFFER_H
