// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/stream/compressor.h"

#include <brotli/encode.h>
#include <brotli/types.h>

#include "brs_ext/buffer.h"
#include "brs_ext/error.h"
#include "brs_ext/gvl.h"
#include "brs_ext/option.h"
#include "ruby.h"

// -- initialization --

static void free_compressor(brs_ext_compressor_t* compressor_ptr)
{
  BrotliEncoderState* state_ptr = compressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliEncoderDestroyInstance(state_ptr);
  }

  brs_ext_byte_t* destination_buffer = compressor_ptr->destination_buffer;
  if (destination_buffer != NULL) {
    free(destination_buffer);
  }

  free(compressor_ptr);
}

VALUE brs_ext_allocate_compressor(VALUE klass)
{
  brs_ext_compressor_t* compressor_ptr;
  VALUE                 self = Data_Make_Struct(klass, brs_ext_compressor_t, NULL, free_compressor, compressor_ptr);

  compressor_ptr->state_ptr                           = NULL;
  compressor_ptr->destination_buffer                  = NULL;
  compressor_ptr->destination_buffer_length           = 0;
  compressor_ptr->remaining_destination_buffer        = NULL;
  compressor_ptr->remaining_destination_buffer_length = 0;
  compressor_ptr->gvl                                 = false;

  return self;
}

#define GET_COMPRESSOR(self)            \
  brs_ext_compressor_t* compressor_ptr; \
  Data_Get_Struct(self, brs_ext_compressor_t, compressor_ptr);

VALUE brs_ext_initialize_compressor(VALUE self, VALUE options)
{
  GET_COMPRESSOR(self);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_COMPRESSOR_OPTIONS(options);
  BRS_EXT_GET_SIZE_OPTION(options, destination_buffer_length);
  BRS_EXT_GET_BOOL_OPTION(options, gvl);

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

  brs_ext_byte_t* destination_buffer = malloc(destination_buffer_length);
  if (destination_buffer == NULL) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(BRS_EXT_ERROR_ALLOCATE_FAILED);
  }

  compressor_ptr->state_ptr                           = state_ptr;
  compressor_ptr->destination_buffer                  = destination_buffer;
  compressor_ptr->destination_buffer_length           = destination_buffer_length;
  compressor_ptr->remaining_destination_buffer        = destination_buffer;
  compressor_ptr->remaining_destination_buffer_length = destination_buffer_length;
  compressor_ptr->gvl                                 = gvl;

  return Qnil;
}

// -- compress --

#define DO_NOT_USE_AFTER_CLOSE(compressor_ptr)                                           \
  if (compressor_ptr->state_ptr == NULL || compressor_ptr->destination_buffer == NULL) { \
    brs_ext_raise_error(BRS_EXT_ERROR_USED_AFTER_CLOSE);                                 \
  }

typedef struct
{
  brs_ext_compressor_t* compressor_ptr;
  const brs_ext_byte_t* remaining_source;
  size_t*               remaining_source_length_ptr;
  BROTLI_BOOL           result;
} compress_args_t;

static inline void* compress_wrapper(void* data)
{
  compress_args_t*      args           = data;
  brs_ext_compressor_t* compressor_ptr = args->compressor_ptr;

  args->result = BrotliEncoderCompressStream(
    compressor_ptr->state_ptr,
    BROTLI_OPERATION_PROCESS,
    args->remaining_source_length_ptr,
    &args->remaining_source,
    &compressor_ptr->remaining_destination_buffer_length,
    &compressor_ptr->remaining_destination_buffer,
    NULL);

  return NULL;
}

VALUE brs_ext_compress(VALUE self, VALUE source_value)
{
  GET_COMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(compressor_ptr);
  Check_Type(source_value, T_STRING);

  const char*           source                  = RSTRING_PTR(source_value);
  size_t                source_length           = RSTRING_LEN(source_value);
  const brs_ext_byte_t* remaining_source        = (const brs_ext_byte_t*) source;
  size_t                remaining_source_length = source_length;

  compress_args_t args = {
    .compressor_ptr              = compressor_ptr,
    .remaining_source            = remaining_source,
    .remaining_source_length_ptr = &remaining_source_length};

  BRS_EXT_GVL_WRAP(compressor_ptr->gvl, compress_wrapper, &args);
  if (!args.result) {
    brs_ext_raise_error(BRS_EXT_ERROR_UNEXPECTED);
  }

  VALUE bytes_written          = SIZET2NUM(source_length - remaining_source_length);
  VALUE needs_more_destination = BrotliEncoderHasMoreOutput(compressor_ptr->state_ptr) ? Qtrue : Qfalse;

  return rb_ary_new_from_args(2, bytes_written, needs_more_destination);
}

// -- compressor flush --

typedef struct
{
  brs_ext_compressor_t* compressor_ptr;
  BROTLI_BOOL           result;
} compressor_flush_args_t;

static inline void* compressor_flush_wrapper(void* data)
{
  compressor_flush_args_t* args                    = data;
  brs_ext_compressor_t*    compressor_ptr          = args->compressor_ptr;
  const brs_ext_byte_t*    remaining_source        = NULL;
  size_t                   remaining_source_length = 0;

  args->result = BrotliEncoderCompressStream(
    compressor_ptr->state_ptr,
    BROTLI_OPERATION_FLUSH,
    &remaining_source_length,
    &remaining_source,
    &compressor_ptr->remaining_destination_buffer_length,
    &compressor_ptr->remaining_destination_buffer,
    NULL);

  return NULL;
}

VALUE brs_ext_flush_compressor(VALUE self)
{
  GET_COMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(compressor_ptr);

  compressor_flush_args_t args = {.compressor_ptr = compressor_ptr};

  BRS_EXT_GVL_WRAP(compressor_ptr->gvl, compressor_flush_wrapper, &args);
  if (!args.result) {
    brs_ext_raise_error(BRS_EXT_ERROR_UNEXPECTED);
  }

  return BrotliEncoderHasMoreOutput(compressor_ptr->state_ptr) ? Qtrue : Qfalse;
}

// -- compressor finish --

typedef struct
{
  brs_ext_compressor_t* compressor_ptr;
  BROTLI_BOOL           result;
} compressor_finish_args_t;

static inline void* compressor_finish_wrapper(void* data)
{
  compressor_finish_args_t* args                    = data;
  brs_ext_compressor_t*     compressor_ptr          = args->compressor_ptr;
  const brs_ext_byte_t*     remaining_source        = NULL;
  size_t                    remaining_source_length = 0;

  args->result = BrotliEncoderCompressStream(
    compressor_ptr->state_ptr,
    BROTLI_OPERATION_FINISH,
    &remaining_source_length,
    &remaining_source,
    &compressor_ptr->remaining_destination_buffer_length,
    &compressor_ptr->remaining_destination_buffer,
    NULL);

  return NULL;
}

VALUE brs_ext_finish_compressor(VALUE self)
{
  GET_COMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(compressor_ptr);

  compressor_finish_args_t args = {.compressor_ptr = compressor_ptr};

  BRS_EXT_GVL_WRAP(compressor_ptr->gvl, compressor_finish_wrapper, &args);
  if (!args.result) {
    brs_ext_raise_error(BRS_EXT_ERROR_UNEXPECTED);
  }

  return (BrotliEncoderHasMoreOutput(compressor_ptr->state_ptr) ||
          !BrotliEncoderIsFinished(compressor_ptr->state_ptr)) ?
           Qtrue :
           Qfalse;
}

// -- other --

VALUE brs_ext_compressor_read_result(VALUE self)
{
  GET_COMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(compressor_ptr);

  brs_ext_byte_t* destination_buffer                  = compressor_ptr->destination_buffer;
  size_t          destination_buffer_length           = compressor_ptr->destination_buffer_length;
  size_t          remaining_destination_buffer_length = compressor_ptr->remaining_destination_buffer_length;

  const char* result        = (const char*) destination_buffer;
  size_t      result_length = destination_buffer_length - remaining_destination_buffer_length;
  VALUE       result_value  = rb_str_new(result, result_length);

  compressor_ptr->remaining_destination_buffer        = destination_buffer;
  compressor_ptr->remaining_destination_buffer_length = destination_buffer_length;

  return result_value;
}

// -- cleanup --

VALUE brs_ext_compressor_close(VALUE self)
{
  GET_COMPRESSOR(self);
  DO_NOT_USE_AFTER_CLOSE(compressor_ptr);

  BrotliEncoderState* state_ptr = compressor_ptr->state_ptr;
  if (state_ptr != NULL) {
    BrotliEncoderDestroyInstance(state_ptr);

    compressor_ptr->state_ptr = NULL;
  }

  brs_ext_byte_t* destination_buffer = compressor_ptr->destination_buffer;
  if (destination_buffer != NULL) {
    free(destination_buffer);

    compressor_ptr->destination_buffer = NULL;
  }

  // It is possible to keep "destination_buffer_length", "remaining_destination_buffer"
  //   and "remaining_destination_buffer_length" as is.

  return Qnil;
}

// -- exports --

void brs_ext_compressor_exports(VALUE root_module)
{
  VALUE module = rb_define_module_under(root_module, "Stream");

  VALUE compressor = rb_define_class_under(module, "NativeCompressor", rb_cObject);

  rb_define_alloc_func(compressor, brs_ext_allocate_compressor);
  rb_define_method(compressor, "initialize", brs_ext_initialize_compressor, 1);
  rb_define_method(compressor, "write", brs_ext_compress, 1);
  rb_define_method(compressor, "flush", brs_ext_flush_compressor, 0);
  rb_define_method(compressor, "finish", brs_ext_finish_compressor, 0);
  rb_define_method(compressor, "read_result", brs_ext_compressor_read_result, 0);
  rb_define_method(compressor, "close", brs_ext_compressor_close, 0);
}
