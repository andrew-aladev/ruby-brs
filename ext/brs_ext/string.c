// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/decode.h>
#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/error.h"
#include "brs_ext/macro.h"
#include "brs_ext/option.h"
#include "brs_ext/string.h"

#define GET_SOURCE_DATA(source_value)                                       \
  Check_Type(source_value, T_STRING);                                       \
                                                                            \
  const uint8_t* source        = (const uint8_t*)RSTRING_PTR(source_value); \
  size_t         source_length = RSTRING_LEN(source_value);

VALUE brs_ext_compress_string(VALUE BRS_EXT_UNUSED(self), VALUE source_value, VALUE options)
{
  BrotliEncoderState* state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  BRS_EXT_PROCESS_COMPRESSOR_OPTIONS(state_ptr, options);
  GET_SOURCE_DATA(source_value);

  VALUE  destination_value                   = rb_str_new(NULL, buffer_length);
  size_t destination_length                  = 0;
  size_t remaining_destination_buffer_length = buffer_length;

  while (true) {
    uint8_t* destination                              = (uint8_t*)RSTRING_PTR(destination_value) + destination_length;
    size_t   prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    BROTLI_BOOL result = BrotliEncoderCompressStream(
      state_ptr,
      BROTLI_OPERATION_FINISH,
      &source_length,
      &source,
      &remaining_destination_buffer_length,
      &destination,
      NULL);

    if (!result) {
      rb_str_free(destination_value);
      BrotliEncoderDestroyInstance(state_ptr);
      brs_ext_raise_error("UnexpectedError", "unexpected error");
    }

    destination_length += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (BrotliEncoderHasMoreOutput(state_ptr) || !BrotliEncoderIsFinished(state_ptr)) {
      destination_value                   = rb_str_resize(destination_value, destination_length + buffer_length);
      remaining_destination_buffer_length = buffer_length;
    }
    else {
      break;
    }
  }

  BrotliEncoderDestroyInstance(state_ptr);

  return rb_str_resize(destination_value, destination_length);
}

VALUE brs_ext_decompress_string(VALUE BRS_EXT_UNUSED(self), VALUE source_value, VALUE options)
{
  BrotliDecoderState* state_ptr = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  BRS_EXT_PROCESS_DECOMPRESSOR_OPTIONS(state_ptr, options);
  GET_SOURCE_DATA(source_value);

  VALUE  destination_value                   = rb_str_new(NULL, buffer_length);
  size_t destination_length                  = 0;
  size_t remaining_destination_buffer_length = buffer_length;

  while (true) {
    uint8_t* destination                              = (uint8_t*)RSTRING_PTR(destination_value) + destination_length;
    size_t   prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    BrotliDecoderResult result = BrotliDecoderDecompressStream(
      state_ptr,
      &source_length,
      &source,
      &remaining_destination_buffer_length,
      &destination,
      NULL);

    if (result != BROTLI_DECODER_RESULT_SUCCESS &&
        result != BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT &&
        result != BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      rb_str_free(destination_value);
      BrotliDecoderDestroyInstance(state_ptr);
      brs_ext_raise_error("UnexpectedError", "unexpected error");
    }

    destination_length += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      destination_value                   = rb_str_resize(destination_value, destination_length + buffer_length);
      remaining_destination_buffer_length = buffer_length;
    }
    else {
      break;
    }
  }

  BrotliDecoderDestroyInstance(state_ptr);

  return rb_str_resize(destination_value, destination_length);
}

void brs_ext_string_exports(VALUE root_module)
{
  rb_define_module_function(root_module, "_native_compress_string", RUBY_METHOD_FUNC(brs_ext_compress_string), 2);
  rb_define_module_function(root_module, "_native_decompress_string", RUBY_METHOD_FUNC(brs_ext_decompress_string), 2);
}
