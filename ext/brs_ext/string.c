// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <stdbool.h>

#include "ruby.h"

#include "brs_ext/buffer.h"
#include "brs_ext/error.h"
#include "brs_ext/macro.h"
#include "brs_ext/option.h"
#include "brs_ext/string.h"

#define GET_STRING(source)                         \
  Check_Type(source, T_STRING);                    \
                                                   \
  const char* source_data   = RSTRING_PTR(source); \
  size_t      source_length = RSTRING_LEN(source);

VALUE brs_ext_compress_string(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE options)
{
  BrotliEncoderState* state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  BRS_EXT_PROCESS_COMPRESSOR_OPTIONS(state_ptr, options);

  uint8_t* buffer = brs_ext_allocate_compressor_buffer(&buffer_length);
  if (buffer == NULL) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  GET_STRING(source);

  while (true) {
    BROTLI_BOOL result = BrotliEncoderCompressStream(
      state_ptr,
      BROTLI_OPERATION_FINISH,
      &source_length,
      &source_data,
      &buffer_length,
      &buffer,
      NULL);

    if (!result) {
      free(buffer);
      BrotliEncoderDestroyInstance(state_ptr);
      brs_ext_raise_error("UnexpectedError", "unexpected error");
    }

    if (!BrotliEncoderHasMoreOutput(state_ptr) && BrotliEncoderIsFinished(state_ptr)) {
      break;
    }
  }

  free(buffer);
  BrotliEncoderDestroyInstance(state_ptr);

  return Qnil;
}

VALUE brs_ext_decompress_string(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE options)
{
  BrotliDecoderState* state_ptr = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  BRS_EXT_PROCESS_DECOMPRESSOR_OPTIONS(state_ptr, options);

  uint8_t* buffer = brs_ext_allocate_decompressor_buffer(&buffer_length);
  if (buffer == NULL) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  GET_STRING(source);

  // while (true) {
  //   BrotliDecoderResult result = BrotliDecoderDecompressStream(
  //     state_ptr,
  //     &source_length,
  //     &source_data,
  //     &buffer_length,
  //     &buffer,
  //     NULL);
  // }

  free(buffer);
  BrotliDecoderDestroyInstance(state_ptr);

  return Qnil;
}

void brs_ext_string_exports(VALUE root_module)
{
  rb_define_module_function(root_module, "_native_compress_string", RUBY_METHOD_FUNC(brs_ext_compress_string), 2);
  rb_define_module_function(root_module, "_native_decompress_string", RUBY_METHOD_FUNC(brs_ext_decompress_string), 2);
}
