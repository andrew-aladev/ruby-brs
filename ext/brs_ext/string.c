// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/decode.h>
#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/error.h"
#include "brs_ext/macro.h"
#include "brs_ext/option.h"
#include "brs_ext/string.h"

#define GET_SOURCE_DATA(source_value)                                 \
  Check_Type(source_value, T_STRING);                                 \
                                                                      \
  const char*    source                  = RSTRING_PTR(source_value); \
  size_t         source_length           = RSTRING_LEN(source_value); \
  const uint8_t* remaining_source        = (const uint8_t*)source;    \
  size_t         remaining_source_length = source_length;

static inline VALUE create_buffer(VALUE length)
{
  return rb_str_new(NULL, NUM2UINT(length));
}

#define CREATE_BUFFER(buffer, length, exception) \
  VALUE buffer = rb_protect(create_buffer, UINT2NUM(length), &exception);

static inline VALUE resize_buffer(VALUE args)
{
  VALUE buffer = rb_ary_entry(args, 0);
  VALUE length = rb_ary_entry(args, 1);
  return rb_str_resize(buffer, NUM2UINT(length));
}

#define RESIZE_BUFFER(buffer, length, exception)                                        \
  VALUE resize_buffer_args = rb_ary_new_from_args(2, buffer, UINT2NUM(length));         \
  buffer                   = rb_protect(resize_buffer, resize_buffer_args, &exception); \
  RB_GC_GUARD(resize_buffer_args);

VALUE brs_ext_compress_string(VALUE BRS_EXT_UNUSED(self), VALUE source_value, VALUE options)
{
  GET_SOURCE_DATA(source_value);

  BrotliEncoderState* state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  BRS_EXT_PROCESS_COMPRESSOR_OPTIONS(state_ptr, options);

  int exception;

  CREATE_BUFFER(destination_value, buffer_length, exception);
  if (exception != 0) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  size_t destination_length                  = 0;
  size_t remaining_destination_buffer_length = buffer_length;

  while (true) {
    uint8_t* destination                              = (uint8_t*)RSTRING_PTR(destination_value) + destination_length;
    size_t   prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    BROTLI_BOOL result = BrotliEncoderCompressStream(
      state_ptr,
      BROTLI_OPERATION_FINISH,
      &remaining_source_length,
      &remaining_source,
      &remaining_destination_buffer_length,
      &destination,
      NULL);

    if (!result) {
      BrotliEncoderDestroyInstance(state_ptr);
      brs_ext_raise_error(BRS_EXT_ERROR_UNEXPECTED);
    }

    destination_length += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (BrotliEncoderHasMoreOutput(state_ptr) || !BrotliEncoderIsFinished(state_ptr)) {
      RESIZE_BUFFER(destination_value, destination_length + buffer_length, exception);
      if (exception != 0) {
        BrotliEncoderDestroyInstance(state_ptr);
        brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
      }

      remaining_destination_buffer_length = buffer_length;
    }
    else {
      break;
    }
  }

  BrotliEncoderDestroyInstance(state_ptr);

  RESIZE_BUFFER(destination_value, destination_length, exception);
  if (exception != 0) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  return destination_value;
}

VALUE brs_ext_decompress_string(VALUE BRS_EXT_UNUSED(self), VALUE source_value, VALUE options)
{
  GET_SOURCE_DATA(source_value);

  BrotliDecoderState* state_ptr = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  BRS_EXT_PROCESS_DECOMPRESSOR_OPTIONS(state_ptr, options);

  int exception;

  CREATE_BUFFER(destination_value, buffer_length, exception);
  if (exception != 0) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  size_t destination_length                  = 0;
  size_t remaining_destination_buffer_length = buffer_length;

  while (true) {
    uint8_t* destination                              = (uint8_t*)RSTRING_PTR(destination_value) + destination_length;
    size_t   prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    BrotliDecoderResult result = BrotliDecoderDecompressStream(
      state_ptr,
      &remaining_source_length,
      &remaining_source,
      &remaining_destination_buffer_length,
      &destination,
      NULL);

    if (
      result != BROTLI_DECODER_RESULT_SUCCESS &&
      result != BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT &&
      result != BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      BrotliDecoderErrorCode error_code = BrotliDecoderGetErrorCode(state_ptr);
      BrotliDecoderDestroyInstance(state_ptr);
      brs_ext_raise_error(brs_ext_get_decompressor_error(error_code));
    }

    destination_length += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      RESIZE_BUFFER(destination_value, destination_length + buffer_length, exception);
      if (exception != 0) {
        BrotliDecoderDestroyInstance(state_ptr);
        brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
      }

      remaining_destination_buffer_length = buffer_length;
    }
    else {
      break;
    }
  }

  BrotliDecoderDestroyInstance(state_ptr);

  RESIZE_BUFFER(destination_value, destination_length, exception);
  if (exception != 0) {
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  return destination_value;
}

void brs_ext_string_exports(VALUE root_module)
{
  rb_define_module_function(root_module, "_native_compress_string", RUBY_METHOD_FUNC(brs_ext_compress_string), 2);
  rb_define_module_function(root_module, "_native_decompress_string", RUBY_METHOD_FUNC(brs_ext_decompress_string), 2);
}
