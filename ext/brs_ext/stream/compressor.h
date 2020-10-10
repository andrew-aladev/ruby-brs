// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_STREAM_COMPRESSOR_H)
#define BRS_EXT_STREAM_COMPRESSOR_H

#include <brotli/encode.h>
#include <stdlib.h>

#include "brs_ext/common.h"
#include "ruby.h"

typedef struct
{
  BrotliEncoderState* state_ptr;
  brs_ext_byte_t*     destination_buffer;
  size_t              destination_buffer_length;
  brs_ext_byte_t*     remaining_destination_buffer;
  size_t              remaining_destination_buffer_length;
} brs_ext_compressor_t;

VALUE brs_ext_allocate_compressor(VALUE klass);
VALUE brs_ext_initialize_compressor(VALUE self, VALUE options);
VALUE brs_ext_compress(VALUE self, VALUE source);
VALUE brs_ext_flush_compressor(VALUE self);
VALUE brs_ext_finish_compressor(VALUE self);
VALUE brs_ext_compressor_read_result(VALUE self);
VALUE brs_ext_compressor_close(VALUE self);

void brs_ext_compressor_exports(VALUE root_module);

#endif // BRS_EXT_STREAM_COMPRESSOR_H
