// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_BUFFER_H)
#define BRS_EXT_BUFFER_H

#include "ruby.h"

#define DEFAULT_COMPRESSOR_BUFFER_LENGTH (1 << 16)   // 32 KB
#define DEFAULT_DECOMPRESSOR_BUFFER_LENGTH (1 << 16) // 64 KB

uint8_t* brs_ext_allocate_compressor_buffer(size_t* buffer_length);
uint8_t* brs_ext_allocate_decompressor_buffer(size_t* buffer_length);

#endif // BRS_EXT_BUFFER_H
