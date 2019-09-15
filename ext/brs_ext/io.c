// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/decode.h>
#include <brotli/encode.h>

#include "ruby.h"
#include "ruby/io.h"

#include "brs_ext/error.h"
#include "brs_ext/io.h"
#include "brs_ext/macro.h"
#include "brs_ext/option.h"

#define GET_FILE(target)                               \
  Check_Type(target, T_FILE);                          \
                                                       \
  rb_io_t *target##_io;                                \
  GetOpenFile(target, target##_io);                    \
                                                       \
  FILE *target##_file = rb_io_stdio_file(target##_io); \
  if (target##_file == NULL) {                         \
    brs_ext_raise_error(BRS_EXT_ERROR_ACCESS_IO);      \
  }

// -- compress --

VALUE brs_ext_compress_io(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE destination, VALUE options)
{
  GET_FILE(source);
  GET_FILE(destination);

  BrotliEncoderState *state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  BRS_EXT_PROCESS_COMPRESSOR_OPTIONS(state_ptr, options);

  uint8_t *source_buffer = malloc(buffer_length);
  if (source_buffer == NULL) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  uint8_t *destination_buffer = malloc(buffer_length);
  if (destination_buffer == NULL) {
    free(source_buffer);
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  ;

  free(source_buffer);
  free(destination_buffer);
  BrotliEncoderDestroyInstance(state_ptr);

  // Ruby itself won't flush stdio file before closing fd, flush is required.
  fflush(destination_file);

  return Qnil;
}

// -- decompress --

VALUE brs_ext_decompress_io(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE destination, VALUE options)
{
  GET_FILE(source);
  GET_FILE(destination);

  BrotliDecoderState *state_ptr = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  BRS_EXT_PROCESS_DECOMPRESSOR_OPTIONS(state_ptr, options);

  uint8_t *source_buffer = malloc(buffer_length);
  if (source_buffer == NULL) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  uint8_t *destination_buffer = malloc(buffer_length);
  if (destination_buffer == NULL) {
    free(source_buffer);
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  ;

  free(source_buffer);
  free(destination_buffer);
  BrotliDecoderDestroyInstance(state_ptr);

  // Ruby itself won't flush stdio file before closing fd, flush is required.
  fflush(destination_file);

  return Qnil;
}

void brs_ext_io_exports(VALUE root_module)
{
  rb_define_module_function(root_module, "_native_compress_io", RUBY_METHOD_FUNC(brs_ext_compress_io), 3);
  rb_define_module_function(root_module, "_native_decompress_io", RUBY_METHOD_FUNC(brs_ext_decompress_io), 3);
}
