// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/string.h"

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <brotli/types.h>
#include <stdlib.h>

#include "brs_ext/buffer.h"
#include "brs_ext/error.h"
#include "brs_ext/macro.h"
#include "brs_ext/option.h"
#include "ruby.h"

// -- buffer --

static inline brs_ext_result_t increase_destination_buffer(
  VALUE   destination_value,
  size_t  destination_length,
  size_t* remaining_destination_buffer_length_ptr,
  size_t  destination_buffer_length)
{
  if (*remaining_destination_buffer_length_ptr == destination_buffer_length) {
    // We want to write more data at once, than buffer has.
    return BRS_EXT_ERROR_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  int exception;

  BRS_EXT_RESIZE_STRING_BUFFER(destination_value, destination_length + destination_buffer_length, exception);
  if (exception != 0) {
    return BRS_EXT_ERROR_ALLOCATE_FAILED;
  }

  *remaining_destination_buffer_length_ptr = destination_buffer_length;

  return 0;
}

// -- compress --

static inline brs_ext_result_t compress(
  BrotliEncoderState* state_ptr,
  const char*         source,
  size_t              source_length,
  VALUE               destination_value,
  size_t              destination_buffer_length)
{
  BROTLI_BOOL           result;
  brs_ext_result_t      ext_result;
  const brs_ext_byte_t* remaining_source                    = (const brs_ext_byte_t*) source;
  size_t                remaining_source_length             = source_length;
  size_t                destination_length                  = 0;
  size_t                remaining_destination_buffer_length = destination_buffer_length;

  while (true) {
    brs_ext_byte_t* remaining_destination_buffer =
      (brs_ext_byte_t*) RSTRING_PTR(destination_value) + destination_length;
    size_t prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    result = BrotliEncoderCompressStream(
      state_ptr,
      BROTLI_OPERATION_FINISH,
      &remaining_source_length,
      &remaining_source,
      &remaining_destination_buffer_length,
      &remaining_destination_buffer,
      NULL);

    if (!result) {
      return BRS_EXT_ERROR_UNEXPECTED;
    }

    destination_length += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (BrotliEncoderHasMoreOutput(state_ptr) || !BrotliEncoderIsFinished(state_ptr)) {
      ext_result = increase_destination_buffer(
        destination_value, destination_length, &remaining_destination_buffer_length, destination_buffer_length);

      if (ext_result != 0) {
        return ext_result;
      }

      continue;
    }

    break;
  }

  int exception;

  BRS_EXT_RESIZE_STRING_BUFFER(destination_value, destination_length, exception);
  if (exception != 0) {
    return BRS_EXT_ERROR_ALLOCATE_FAILED;
  }

  return 0;
}

VALUE brs_ext_compress_string(VALUE BRS_EXT_UNUSED(self), VALUE source_value, VALUE options)
{
  Check_Type(source_value, T_STRING);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_SIZE_OPTION(options, destination_buffer_length);
  BRS_EXT_GET_COMPRESSOR_OPTIONS(options);

  BrotliEncoderState* state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  brs_ext_result_t ext_result = brs_ext_set_compressor_options(state_ptr, &compressor_options);
  if (ext_result != 0) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  if (destination_buffer_length == 0) {
    destination_buffer_length = BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_COMPRESSOR;
  }

  int exception;

  BRS_EXT_CREATE_STRING_BUFFER(destination_value, destination_buffer_length, exception);
  if (exception != 0) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  const char* source        = RSTRING_PTR(source_value);
  size_t      source_length = RSTRING_LEN(source_value);

  ext_result = compress(state_ptr, source, source_length, destination_value, destination_buffer_length);

  BrotliEncoderDestroyInstance(state_ptr);

  if (ext_result != 0) {
    brs_ext_raise_error(ext_result);
  }

  return destination_value;
}

// -- decompress --

static inline brs_ext_result_t decompress(
  BrotliDecoderState* state_ptr,
  const char*         source,
  size_t              source_length,
  VALUE               destination_value,
  size_t              destination_buffer_length)
{
  BrotliDecoderResult   result;
  brs_ext_result_t      ext_result;
  const brs_ext_byte_t* remaining_source                    = (const brs_ext_byte_t*) source;
  size_t                remaining_source_length             = source_length;
  size_t                destination_length                  = 0;
  size_t                remaining_destination_buffer_length = destination_buffer_length;

  while (true) {
    brs_ext_byte_t* remaining_destination_buffer =
      (brs_ext_byte_t*) RSTRING_PTR(destination_value) + destination_length;
    size_t prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    result = BrotliDecoderDecompressStream(
      state_ptr,
      &remaining_source_length,
      &remaining_source,
      &remaining_destination_buffer_length,
      &remaining_destination_buffer,
      NULL);

    if (
      result != BROTLI_DECODER_RESULT_SUCCESS && result != BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT &&
      result != BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      BrotliDecoderErrorCode error_code = BrotliDecoderGetErrorCode(state_ptr);
      return brs_ext_get_decompressor_error(error_code);
    }

    destination_length += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      ext_result = increase_destination_buffer(
        destination_value, destination_length, &remaining_destination_buffer_length, destination_buffer_length);

      if (ext_result != 0) {
        return ext_result;
      }

      continue;
    }

    break;
  }

  int exception;

  BRS_EXT_RESIZE_STRING_BUFFER(destination_value, destination_length, exception);
  if (exception != 0) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  return 0;
}

VALUE brs_ext_decompress_string(VALUE BRS_EXT_UNUSED(self), VALUE source_value, VALUE options)
{
  Check_Type(source_value, T_STRING);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_SIZE_OPTION(options, destination_buffer_length);
  BRS_EXT_GET_DECOMPRESSOR_OPTIONS(options);

  BrotliDecoderState* state_ptr = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  brs_ext_result_t ext_result = brs_ext_set_decompressor_options(state_ptr, &decompressor_options);
  if (ext_result != 0) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  if (destination_buffer_length == 0) {
    destination_buffer_length = BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_DECOMPRESSOR;
  }

  int exception;

  BRS_EXT_CREATE_STRING_BUFFER(destination_value, destination_buffer_length, exception);
  if (exception != 0) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  const char* source        = RSTRING_PTR(source_value);
  size_t      source_length = RSTRING_LEN(source_value);

  ext_result = decompress(state_ptr, source, source_length, destination_value, destination_buffer_length);

  BrotliDecoderDestroyInstance(state_ptr);

  if (ext_result != 0) {
    brs_ext_raise_error(ext_result);
  }

  return destination_value;
}

// -- exports --

void brs_ext_string_exports(VALUE root_module)
{
  rb_define_module_function(root_module, "_native_compress_string", RUBY_METHOD_FUNC(brs_ext_compress_string), 2);
  rb_define_module_function(root_module, "_native_decompress_string", RUBY_METHOD_FUNC(brs_ext_decompress_string), 2);
}
