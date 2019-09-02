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

#define GET_COMPRESSOR()                \
  brs_ext_compressor_t* compressor_ptr; \
  Data_Get_Struct(self, brs_ext_compressor_t, compressor_ptr);

VALUE brs_ext_initialize_compressor(VALUE self, VALUE options)
{
  GET_COMPRESSOR();

  BrotliEncoderState* state_ptr = BrotliEncoderCreateInstance(NULL, NULL, NULL);
  if (state_ptr == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  compressor_ptr->state_ptr = state_ptr;

  BRS_EXT_PROCESS_COMPRESSOR_OPTIONS(options, state_ptr);

  // -----

  if (buffer_length == 0) {
    buffer_length = DEFAULT_COMPRESSOR_BUFFER_LENGTH;
  }

  uint8_t* buffer = malloc(buffer_length);
  if (buffer == NULL) {
    brs_ext_raise_error("AllocateError", "allocate error");
  }

  compressor_ptr->destination_buffer                  = buffer;
  compressor_ptr->destination_buffer_length           = buffer_length;
  compressor_ptr->remaining_destination_buffer        = buffer;
  compressor_ptr->remaining_destination_buffer_length = buffer_length;

  return Qnil;
}

#define DO_NOT_USE_AFTER_CLOSE()                                                         \
  if (compressor_ptr->state_ptr == NULL || compressor_ptr->destination_buffer == NULL) { \
    brs_ext_raise_error("UsedAfterCloseError", "compressor used after closed");          \
  }

#define GET_SOURCE_STRING()                        \
  Check_Type(source, T_STRING);                    \
                                                   \
  const char* source_data   = RSTRING_PTR(source); \
  size_t      source_length = RSTRING_LEN(source);

VALUE brs_ext_compress(VALUE self, VALUE source)
{
  GET_COMPRESSOR();
  DO_NOT_USE_AFTER_CLOSE();
  GET_SOURCE_STRING();

  const uint8_t* remaining_source_data   = (const uint8_t*)source_data;
  size_t         remaining_source_length = source_length;

  BROTLI_BOOL result = BrotliEncoderCompressStream(
    compressor_ptr->state_ptr,
    BROTLI_OPERATION_PROCESS,
    &remaining_source_length,
    &remaining_source_data,
    &compressor_ptr->remaining_destination_buffer_length,
    &compressor_ptr->remaining_destination_buffer,
    NULL);

  if (!result) {
    brs_ext_raise_error("UnexpectedError", "unexpected error");
  }

  VALUE bytes_written          = INT2NUM(source_length - remaining_source_length);
  VALUE needs_more_destination = BrotliEncoderHasMoreOutput(compressor_ptr->state_ptr) ? Qtrue : Qfalse;

  return rb_ary_new_from_args(2, bytes_written, needs_more_destination);
}

VALUE brs_ext_flush_compressor(VALUE self)
{
  GET_COMPRESSOR();
  DO_NOT_USE_AFTER_CLOSE();

  const uint8_t* remaining_source_data   = NULL;
  size_t         remaining_source_length = 0;

  BROTLI_BOOL result = BrotliEncoderCompressStream(
    compressor_ptr->state_ptr,
    BROTLI_OPERATION_FLUSH,
    &remaining_source_length,
    &remaining_source_data,
    &compressor_ptr->remaining_destination_buffer_length,
    &compressor_ptr->remaining_destination_buffer,
    NULL);

  if (!result) {
    brs_ext_raise_error("UnexpectedError", "unexpected error");
  }

  return BrotliEncoderHasMoreOutput(compressor_ptr->state_ptr) ? Qtrue : Qfalse;
}

VALUE brs_ext_finish_compressor(VALUE self)
{
  GET_COMPRESSOR();
  DO_NOT_USE_AFTER_CLOSE();

  const uint8_t* remaining_source_data   = NULL;
  size_t         remaining_source_length = 0;

  BROTLI_BOOL result = BrotliEncoderCompressStream(
    compressor_ptr->state_ptr,
    BROTLI_OPERATION_FINISH,
    &remaining_source_length,
    &remaining_source_data,
    &compressor_ptr->remaining_destination_buffer_length,
    &compressor_ptr->remaining_destination_buffer,
    NULL);

  if (!result) {
    brs_ext_raise_error("UnexpectedError", "unexpected error");
  }

  return BrotliEncoderHasMoreOutput(compressor_ptr->state_ptr) ? Qtrue : Qfalse;
}

VALUE brs_ext_compressor_read_result(VALUE self)
{
  GET_COMPRESSOR();
  DO_NOT_USE_AFTER_CLOSE();

  uint8_t* destination_buffer                  = compressor_ptr->destination_buffer;
  size_t   destination_buffer_length           = compressor_ptr->destination_buffer_length;
  size_t   remaining_destination_buffer_length = compressor_ptr->remaining_destination_buffer_length;

  const char* result_data   = (const char*)destination_buffer;
  size_t      result_length = destination_buffer_length - remaining_destination_buffer_length;

  VALUE result = rb_str_new(result_data, result_length);

  compressor_ptr->remaining_destination_buffer        = destination_buffer;
  compressor_ptr->remaining_destination_buffer_length = destination_buffer_length;

  return result;
}

VALUE brs_ext_compressor_close(VALUE self)
{
  GET_COMPRESSOR();
  DO_NOT_USE_AFTER_CLOSE();

  BrotliEncoderState* state_ptr = compressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliEncoderDestroyInstance(state_ptr);

    compressor_ptr->state_ptr = NULL;
  }

  uint8_t* destination_buffer = compressor_ptr->destination_buffer;
  if (destination_buffer != NULL) {
    free(destination_buffer);

    compressor_ptr->destination_buffer = NULL;
  }

  // It is possible to keep "destination_buffer_length", "remaining_destination_buffer"
  //   and "remaining_destination_buffer_length" as is.

  return Qnil;
}

void brs_ext_compressor_exports(VALUE root_module)
{
  VALUE stream = rb_define_module_under(root_module, "Stream");

  VALUE compressor = rb_define_class_under(stream, "NativeCompressor", rb_cObject);
  rb_define_alloc_func(compressor, brs_ext_allocate_compressor);
  rb_define_method(compressor, "initialize", brs_ext_initialize_compressor, 1);
  rb_define_method(compressor, "write", brs_ext_compress, 1);
  rb_define_method(compressor, "flush", brs_ext_flush_compressor, 0);
  rb_define_method(compressor, "finish", brs_ext_finish_compressor, 0);
  rb_define_method(compressor, "read_result", brs_ext_compressor_read_result, 0);
  rb_define_method(compressor, "close", brs_ext_compressor_close, 0);
}
