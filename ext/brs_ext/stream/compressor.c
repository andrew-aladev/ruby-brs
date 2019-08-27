// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/stream/compressor.h"

static void free_compressor(brs_ext_compressor_t* compressor_ptr)
{
  BrotliEncoderState* state_ptr = compressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliEncoderDestroyInstance(state_ptr);
  }

  free(compressor_ptr);
}

VALUE brs_ext_allocate_compressor(VALUE klass)
{
  brs_ext_compressor_t* compressor_ptr;

  VALUE self = Data_Make_Struct(klass, brs_ext_compressor_t, NULL, free_compressor, compressor_ptr);

  compressor_ptr->state_ptr = NULL;

  return self;
}
