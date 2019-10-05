// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_ERROR_H)
#define BRS_EXT_ERROR_H

#include <brotli/decode.h>

#include "brs_ext/common.h"
#include "ruby.h"

// Results for errors listed in "lib/brs/error" used in c extension.

enum {
  BRS_EXT_ERROR_ALLOCATE_FAILED = 1,
  BRS_EXT_ERROR_VALIDATE_FAILED,

  BRS_EXT_ERROR_USED_AFTER_CLOSE,
  BRS_EXT_ERROR_NOT_ENOUGH_SOURCE_BUFFER,
  BRS_EXT_ERROR_NOT_ENOUGH_DESTINATION_BUFFER,
  BRS_EXT_ERROR_DECOMPRESSOR_CORRUPTED_SOURCE,

  BRS_EXT_ERROR_ACCESS_IO,
  BRS_EXT_ERROR_READ_IO,
  BRS_EXT_ERROR_WRITE_IO,

  BRS_EXT_ERROR_UNEXPECTED
};

brs_ext_result_t brs_ext_get_decompressor_error(BrotliDecoderErrorCode error_code);

NORETURN(void brs_ext_raise_error(brs_ext_result_t result));

#endif // BRS_EXT_ERROR_H
