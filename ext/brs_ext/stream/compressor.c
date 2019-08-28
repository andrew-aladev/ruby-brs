// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/error.h"
#include "brs_ext/option.h"
#include "brs_ext/stream/compressor.h"

static void free_compressor(brs_ext_compressor_t* compressor_ptr)
{
  BrotliEncoderState* state_ptr = compressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliEncoderDestroyInstance(state_ptr);
  }

  uint8_t* destination_buffer = compressor_ptr->destination_buffer;
  if (destination_buffer != NULL) {
    free(destination_buffer);
  }

  free(compressor_ptr);
}

VALUE brs_ext_allocate_compressor(VALUE klass)
{
  brs_ext_compressor_t* compressor_ptr;

  VALUE self = Data_Make_Struct(klass, brs_ext_compressor_t, NULL, free_compressor, compressor_ptr);

  compressor_ptr->state_ptr                           = NULL;
  compressor_ptr->destination_buffer                  = NULL;
  compressor_ptr->destination_buffer_length           = 0;
  compressor_ptr->remaining_destination_buffer        = NULL;
  compressor_ptr->remaining_destination_buffer_length = 0;

  return self;
}

#define GET_COMPRESSOR(self)            \
  brs_ext_compressor_t* compressor_ptr; \
  Data_Get_Struct(self, brs_ext_compressor_t, compressor_ptr);

VALUE brs_ext_initialize_compressor(VALUE self, VALUE options)
{
  GET_COMPRESSOR(self);
  BRS_EXT_GET_COMPRESSOR_OPTIONS(options);

  BrotliEncoderState* state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  compressor_ptr->state_ptr = state_ptr;

  // -----

  return Qnil;
}
