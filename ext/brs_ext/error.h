// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_ERROR_H)
#define BRS_EXT_ERROR_H

#include <brotli/decode.h>

#include "ruby.h"

enum {
  BRS_EXT_DECOMPRESSOR_CORRUPTED_SOURCE = 1,
  BRS_EXT_DECOMPRESSOR_ALLOCATE_FAILED,
  BRS_EXT_DECOMPRESSOR_UNEXPECTED_ERROR
};

typedef uint8_t brs_ext_decompressor_error_t;

brs_ext_decompressor_error_t brs_ext_get_decompressor_error(BrotliDecoderErrorCode error_code);
NORETURN(void brs_ext_raise_error(const char* name, const char* description));

#endif // BRS_EXT_ERROR_H
