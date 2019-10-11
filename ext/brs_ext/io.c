// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "ruby/io.h"

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <stdint.h>
#include <stdlib.h>

#include "brs_ext/buffer.h"
#include "brs_ext/common.h"
#include "brs_ext/error.h"
#include "brs_ext/io.h"
#include "brs_ext/macro.h"
#include "brs_ext/option.h"
#include "ruby.h"

#define GET_FILE(target)                               \
  Check_Type(target, T_FILE);                          \
                                                       \
  rb_io_t* target##_io;                                \
  GetOpenFile(target, target##_io);                    \
                                                       \
  FILE* target##_file = rb_io_stdio_file(target##_io); \
  if (target##_file == NULL) {                         \
    brs_ext_raise_error(BRS_EXT_ERROR_ACCESS_IO);      \
  }

// -- buffer --

static inline brs_ext_result_t create_buffers(
  uint8_t** source_buffer_ptr, size_t source_buffer_length,
  uint8_t** destination_buffer_ptr, size_t destination_buffer_length)
{
  uint8_t* source_buffer = malloc(source_buffer_length);
  if (source_buffer == NULL) {
    return BRS_EXT_ERROR_ALLOCATE_FAILED;
  }

  uint8_t* destination_buffer = malloc(destination_buffer_length);
  if (destination_buffer == NULL) {
    free(source_buffer);
    return BRS_EXT_ERROR_ALLOCATE_FAILED;
  }

  *source_buffer_ptr      = source_buffer;
  *destination_buffer_ptr = destination_buffer;

  return 0;
}

// -- compress --

static inline brs_ext_result_t compress_data(
  BrotliEncoderState* state_ptr,
  FILE* source_file, uint8_t* source_buffer, size_t source_buffer_length,
  FILE* destination_file, uint8_t* destination_buffer, size_t destination_buffer_length)
{
  return 0;
}

VALUE brs_ext_compress_io(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE destination, VALUE options)
{
  GET_FILE(source);
  GET_FILE(destination);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_COMPRESSOR_OPTIONS(options);
  BRS_EXT_GET_BUFFER_LENGTH_OPTION(options, source_buffer_length);
  BRS_EXT_GET_BUFFER_LENGTH_OPTION(options, destination_buffer_length);

  BrotliEncoderState* state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  brs_ext_result_t ext_result = brs_ext_set_compressor_options(state_ptr, &compressor_options);
  if (ext_result != 0) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  if (source_buffer_length == 0) {
    source_buffer_length = BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_COMPRESSOR;
  }
  if (destination_buffer_length == 0) {
    destination_buffer_length = BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_COMPRESSOR;
  }

  uint8_t* source_buffer;
  uint8_t* destination_buffer;

  ext_result = create_buffers(
    &source_buffer, source_buffer_length,
    &destination_buffer, destination_buffer_length);

  if (ext_result != 0) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  ext_result = compress_data(
    state_ptr,
    source_file, source_buffer, source_buffer_length,
    destination_file, destination_buffer, destination_buffer_length);

  free(source_buffer);
  free(destination_buffer);
  BrotliEncoderDestroyInstance(state_ptr);

  if (ext_result != 0) {
    brs_ext_raise_error(ext_result);
  }

  // Ruby itself won't flush stdio file before closing fd, flush is required.
  fflush(destination_file);

  return Qnil;
}

// -- decompress --

static inline brs_ext_result_t decompress_data(
  BrotliDecoderState* state_ptr,
  FILE* source_file, uint8_t* source_buffer, size_t source_buffer_length,
  FILE* destination_file, uint8_t* destination_buffer, size_t destination_buffer_length)
{
  return 0;
}

VALUE brs_ext_decompress_io(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE destination, VALUE options)
{
  GET_FILE(source);
  GET_FILE(destination);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_DECOMPRESSOR_OPTIONS(options);
  BRS_EXT_GET_BUFFER_LENGTH_OPTION(options, source_buffer_length);
  BRS_EXT_GET_BUFFER_LENGTH_OPTION(options, destination_buffer_length);

  BrotliDecoderState* state_ptr = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  brs_ext_result_t ext_result = brs_ext_set_decompressor_options(state_ptr, &decompressor_options);
  if (ext_result != 0) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  if (source_buffer_length == 0) {
    source_buffer_length = BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_DECOMPRESSOR;
  }
  if (destination_buffer_length == 0) {
    destination_buffer_length = BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_DECOMPRESSOR;
  }

  uint8_t* source_buffer;
  uint8_t* destination_buffer;

  ext_result = create_buffers(
    &source_buffer, source_buffer_length,
    &destination_buffer, destination_buffer_length);

  if (ext_result != 0) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  ext_result = decompress_data(
    state_ptr,
    source_file, source_buffer, source_buffer_length,
    destination_file, destination_buffer, destination_buffer_length);

  free(source_buffer);
  free(destination_buffer);
  BrotliDecoderDestroyInstance(state_ptr);

  if (ext_result != 0) {
    brs_ext_raise_error(ext_result);
  }

  // Ruby itself won't flush stdio file before closing fd, flush is required.
  fflush(destination_file);

  return Qnil;
}

void brs_ext_io_exports(VALUE root_module)
{
  rb_define_module_function(root_module, "_native_compress_io", RUBY_METHOD_FUNC(brs_ext_compress_io), 3);
  rb_define_module_function(root_module, "_native_decompress_io", RUBY_METHOD_FUNC(brs_ext_decompress_io), 3);
}
