// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/stream/decompressor.h"

#include <brotli/decode.h>

#include "brs_ext/buffer.h"
#include "brs_ext/error.h"
#include "brs_ext/gvl.h"
#include "brs_ext/option.h"
#include "ruby.h"

// -- initialization --

static void free_decompressor(brs_ext_decompressor_t* decompressor_ptr)
{
  BrotliDecoderState* state_ptr = decompressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliDecoderDestroyInstance(state_ptr);
  }

  brs_ext_byte_t* destination_buffer = decompressor_ptr->destination_buffer;
  if (destination_buffer != NULL) {
    free(destination_buffer);
  }

  free(decompressor_ptr);
}

VALUE brs_ext_allocate_decompressor(VALUE klass)
{
  brs_ext_decompressor_t* decompressor_ptr;
  VALUE self = Data_Make_Struct(klass, brs_ext_decompressor_t, NULL, free_decompressor, decompressor_ptr);

  decompressor_ptr->state_ptr                           = NULL;
  decompressor_ptr->destination_buffer                  = NULL;
  decompressor_ptr->destination_buffer_length           = 0;
  decompressor_ptr->remaining_destination_buffer        = NULL;
  decompressor_ptr->remaining_destination_buffer_length = 0;

  return self;
}

#define GET_DECOMPRESSOR(self)              \
  brs_ext_decompressor_t* decompressor_ptr; \
  Data_Get_Struct(self, brs_ext_decompressor_t, decompressor_ptr);

VALUE brs_ext_initialize_decompressor(VALUE self, VALUE options)
{
  GET_DECOMPRESSOR(self);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_SIZE_OPTION(options, destination_buffer_length);
  BRS_EXT_GET_BOOL_OPTION(options, gvl);
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

  brs_ext_byte_t* destination_buffer = malloc(destination_buffer_length);
  if (destination_buffer == NULL) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  decompressor_ptr->state_ptr                           = state_ptr;
  decompressor_ptr->destination_buffer                  = destination_buffer;
  decompressor_ptr->destination_buffer_length           = destination_buffer_length;
  decompressor_ptr->remaining_destination_buffer        = destination_buffer;
  decompressor_ptr->remaining_destination_buffer_length = destination_buffer_length;
  decompressor_ptr->gvl                                 = gvl;

  return Qnil;
}

// -- decompress --

#define DO_NOT_USE_AFTER_CLOSE(decompressor_ptr)                                             \
  if (decompressor_ptr->state_ptr == NULL || decompressor_ptr->destination_buffer == NULL) { \
    brs_ext_raise_error(BRS_EXT_ERROR_USED_AFTER_CLOSE);                                     \
  }

typedef struct
{
  brs_ext_decompressor_t* decompressor_ptr;
  const brs_ext_byte_t*   remaining_source;
  size_t*                 remaining_source_length_ptr;
  BrotliDecoderResult     result;
} decompress_args_t;

static inline void* decompress_wrapper(void* data)
{
  decompress_args_t*      args             = data;
  brs_ext_decompressor_t* decompressor_ptr = args->decompressor_ptr;

  args->result = BrotliDecoderDecompressStream(
    decompressor_ptr->state_ptr,
    args->remaining_source_length_ptr,
    &args->remaining_source,
    &decompressor_ptr->remaining_destination_buffer_length,
    &decompressor_ptr->remaining_destination_buffer,
    NULL);

  return NULL;
}

VALUE brs_ext_decompress(VALUE self, VALUE source_value)
{
  GET_DECOMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(decompressor_ptr);
  Check_Type(source_value, T_STRING);

  const char*           source                  = RSTRING_PTR(source_value);
  size_t                source_length           = RSTRING_LEN(source_value);
  const brs_ext_byte_t* remaining_source        = (const brs_ext_byte_t*) source;
  size_t                remaining_source_length = source_length;

  decompress_args_t args = {
    .decompressor_ptr            = decompressor_ptr,
    .remaining_source            = remaining_source,
    .remaining_source_length_ptr = &remaining_source_length};

  BRS_EXT_GVL_WRAP(decompressor_ptr->gvl, decompress_wrapper, &args);

  if (
    args.result != BROTLI_DECODER_RESULT_SUCCESS && args.result != BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT &&
    args.result != BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
    BrotliDecoderErrorCode error_code = BrotliDecoderGetErrorCode(decompressor_ptr->state_ptr);
    brs_ext_raise_error(brs_ext_get_decompressor_error(error_code));
  }

  VALUE bytes_read             = SIZET2NUM(source_length - remaining_source_length);
  VALUE needs_more_destination = args.result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT ? Qtrue : Qfalse;

  return rb_ary_new_from_args(2, bytes_read, needs_more_destination);
}

// -- other --

VALUE brs_ext_decompressor_read_result(VALUE self)
{
  GET_DECOMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(decompressor_ptr);

  brs_ext_byte_t* destination_buffer                  = decompressor_ptr->destination_buffer;
  size_t          destination_buffer_length           = decompressor_ptr->destination_buffer_length;
  size_t          remaining_destination_buffer_length = decompressor_ptr->remaining_destination_buffer_length;

  const char* result        = (const char*) destination_buffer;
  size_t      result_length = destination_buffer_length - remaining_destination_buffer_length;
  VALUE       result_value  = rb_str_new(result, result_length);

  decompressor_ptr->remaining_destination_buffer        = destination_buffer;
  decompressor_ptr->remaining_destination_buffer_length = destination_buffer_length;

  return result_value;
}

// -- cleanup --

VALUE brs_ext_decompressor_close(VALUE self)
{
  GET_DECOMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(decompressor_ptr);

  BrotliDecoderState* state_ptr = decompressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliDecoderDestroyInstance(state_ptr);

    decompressor_ptr->state_ptr = NULL;
  }

  brs_ext_byte_t* destination_buffer = decompressor_ptr->destination_buffer;
  if (destination_buffer != NULL) {
    free(destination_buffer);

    decompressor_ptr->destination_buffer = NULL;
  }

  // It is possible to keep "destination_buffer_length", "remaining_destination_buffer"
  //   and "remaining_destination_buffer_length" as is.

  return Qnil;
}

// -- exports --

void brs_ext_decompressor_exports(VALUE root_module)
{
  VALUE module = rb_define_module_under(root_module, "Stream");

  VALUE decompressor = rb_define_class_under(module, "NativeDecompressor", rb_cObject);

  rb_define_alloc_func(decompressor, brs_ext_allocate_decompressor);
  rb_define_method(decompressor, "initialize", brs_ext_initialize_decompressor, 1);
  rb_define_method(decompressor, "read", brs_ext_decompress, 1);
  rb_define_method(decompressor, "read_result", brs_ext_decompressor_read_result, 0);
  rb_define_method(decompressor, "close", brs_ext_decompressor_close, 0);
}
