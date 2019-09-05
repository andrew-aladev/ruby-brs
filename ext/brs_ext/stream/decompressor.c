// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/decode.h>

#include "ruby.h"

#include "brs_ext/error.h"
#include "brs_ext/option.h"
#include "brs_ext/stream/decompressor.h"

static void free_decompressor(brs_ext_decompressor_t* decompressor_ptr)
{
  BrotliDecoderState* state_ptr = decompressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliDecoderDestroyInstance(state_ptr);
  }

  uint8_t* destination_buffer = decompressor_ptr->destination_buffer;
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

  BrotliDecoderState* state_ptr = BrotliDecoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  BRS_EXT_PROCESS_DECOMPRESSOR_OPTIONS(state_ptr, options);

  uint8_t* buffer = malloc(buffer_length);
  if (buffer == NULL) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  decompressor_ptr->state_ptr                           = state_ptr;
  decompressor_ptr->destination_buffer                  = buffer;
  decompressor_ptr->destination_buffer_length           = buffer_length;
  decompressor_ptr->remaining_destination_buffer        = buffer;
  decompressor_ptr->remaining_destination_buffer_length = buffer_length;

  return Qnil;
}

#define DO_NOT_USE_AFTER_CLOSE(decompressor_ptr)                                             \
  if (decompressor_ptr->state_ptr == NULL || decompressor_ptr->destination_buffer == NULL) { \
    brs_ext_raise_error("UsedAfterCloseError", "decompressor used after closed");            \
  }

#define GET_SOURCE_DATA(source_value)                                 \
  Check_Type(source_value, T_STRING);                                 \
                                                                      \
  const char*    source                  = RSTRING_PTR(source_value); \
  size_t         source_length           = RSTRING_LEN(source_value); \
  const uint8_t* remaining_source        = (const uint8_t*)source;    \
  size_t         remaining_source_length = source_length;

VALUE brs_ext_decompress(VALUE self, VALUE source_value)
{
  GET_DECOMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(decompressor_ptr);
  GET_SOURCE_DATA(source_value);

  BrotliDecoderResult result = BrotliDecoderDecompressStream(
    decompressor_ptr->state_ptr,
    &remaining_source_length,
    &remaining_source,
    &decompressor_ptr->remaining_destination_buffer_length,
    &decompressor_ptr->remaining_destination_buffer,
    NULL);

  VALUE bytes_written = INT2NUM(source_length - remaining_source_length);

  VALUE needs_more_destination;
  if (result == BROTLI_DECODER_RESULT_SUCCESS || result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT) {
    needs_more_destination = Qfalse;
  }
  else if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
    needs_more_destination = Qtrue;
  }
  else {
    brs_ext_raise_error("UnexpectedError", "unexpected error");
  }

  return rb_ary_new_from_args(2, bytes_written, needs_more_destination);
}

VALUE brs_ext_decompressor_read_result(VALUE self)
{
  GET_DECOMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(decompressor_ptr);

  uint8_t* destination_buffer                  = decompressor_ptr->destination_buffer;
  size_t   destination_buffer_length           = decompressor_ptr->destination_buffer_length;
  size_t   remaining_destination_buffer_length = decompressor_ptr->remaining_destination_buffer_length;

  const char* result        = (const char*)destination_buffer;
  size_t      result_length = destination_buffer_length - remaining_destination_buffer_length;

  VALUE result_value = rb_str_new(result, result_length);

  decompressor_ptr->remaining_destination_buffer        = destination_buffer;
  decompressor_ptr->remaining_destination_buffer_length = destination_buffer_length;

  return result_value;
}

VALUE brs_ext_decompressor_close(VALUE self)
{
  GET_DECOMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(decompressor_ptr);

  BrotliDecoderState* state_ptr = decompressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliDecoderDestroyInstance(state_ptr);

    decompressor_ptr->state_ptr = NULL;
  }

  uint8_t* destination_buffer = decompressor_ptr->destination_buffer;
  if (destination_buffer != NULL) {
    free(destination_buffer);

    decompressor_ptr->destination_buffer = NULL;
  }

  // It is possible to keep "destination_buffer_length", "remaining_destination_buffer"
  //   and "remaining_destination_buffer_length" as is.

  return Qnil;
}

void brs_ext_decompressor_exports(VALUE root_module)
{
  VALUE stream = rb_define_module_under(root_module, "Stream");

  VALUE decompressor = rb_define_class_under(stream, "NativeDecompressor", rb_cObject);
  rb_define_alloc_func(decompressor, brs_ext_allocate_decompressor);
  rb_define_method(decompressor, "initialize", brs_ext_initialize_decompressor, 1);
  rb_define_method(decompressor, "read", brs_ext_decompress, 1);
  rb_define_method(decompressor, "read_result", brs_ext_decompressor_read_result, 0);
  rb_define_method(decompressor, "close", brs_ext_decompressor_close, 0);
}
