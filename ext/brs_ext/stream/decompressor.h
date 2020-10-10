// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_STREAM_DECOMPRESSOR_H)
#define BRS_EXT_STREAM_DECOMPRESSOR_H

#include <brotli/decode.h>
#include <stdlib.h>

#include "brs_ext/common.h"
#include "ruby.h"

typedef struct
{
  BrotliDecoderState* state_ptr;
  brs_ext_byte_t*     destination_buffer;
  size_t              destination_buffer_length;
  brs_ext_byte_t*     remaining_destination_buffer;
  size_t              remaining_destination_buffer_length;
} brs_ext_decompressor_t;

VALUE brs_ext_allocate_decompressor(VALUE klass);
VALUE brs_ext_initialize_decompressor(VALUE self, VALUE options);
VALUE brs_ext_decompress(VALUE self, VALUE source);
VALUE brs_ext_decompressor_read_result(VALUE self);
VALUE brs_ext_decompressor_close(VALUE self);

void brs_ext_decompressor_exports(VALUE root_module);

#endif // BRS_EXT_STREAM_DECOMPRESSOR_H
