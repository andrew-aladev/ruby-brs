// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/io.h"

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <brotli/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brs_ext/buffer.h"
#include "brs_ext/error.h"
#include "brs_ext/gvl.h"
#include "brs_ext/macro.h"
#include "brs_ext/option.h"
#include "ruby/io.h"

// Additional possible results:
enum
{
  BRS_EXT_FILE_READ_FINISHED = 128
};

// -- file --

static inline brs_ext_result_t
  read_file(FILE* source_file, brs_ext_byte_t* source_buffer, size_t* source_length_ptr, size_t source_buffer_length)
{
  size_t read_length = fread(source_buffer, 1, source_buffer_length, source_file);
  if (read_length == 0 && feof(source_file)) {
    return BRS_EXT_FILE_READ_FINISHED;
  }

  if (read_length != source_buffer_length && ferror(source_file)) {
    return BRS_EXT_ERROR_READ_IO;
  }

  *source_length_ptr = read_length;

  return 0;
}

static inline brs_ext_result_t
  write_file(FILE* destination_file, brs_ext_byte_t* destination_buffer, size_t destination_length)
{
  size_t written_length = fwrite(destination_buffer, 1, destination_length, destination_file);
  if (written_length != destination_length) {
    return BRS_EXT_ERROR_WRITE_IO;
  }

  return 0;
}

// -- buffer --

static inline brs_ext_result_t create_buffers(
  brs_ext_byte_t** source_buffer_ptr,
  size_t           source_buffer_length,
  brs_ext_byte_t** destination_buffer_ptr,
  size_t           destination_buffer_length)
{
  brs_ext_byte_t* source_buffer = malloc(source_buffer_length);
  if (source_buffer == NULL) {
    return BRS_EXT_ERROR_ALLOCATE_FAILED;
  }

  brs_ext_byte_t* destination_buffer = malloc(destination_buffer_length);
  if (destination_buffer == NULL) {
    free(source_buffer);
    return BRS_EXT_ERROR_ALLOCATE_FAILED;
  }

  *source_buffer_ptr      = source_buffer;
  *destination_buffer_ptr = destination_buffer;

  return 0;
}

// We have read some source from file into source buffer.
// Than algorithm has read part of this source.
// We need to move remaining source to the top of source buffer.
// Than we can read more source from file.
// Algorithm can use same buffer again.

static inline brs_ext_result_t read_more_source(
  FILE*                  source_file,
  const brs_ext_byte_t** source_ptr,
  size_t*                source_length_ptr,
  brs_ext_byte_t*        source_buffer,
  size_t                 source_buffer_length)
{
  const brs_ext_byte_t* source        = *source_ptr;
  size_t                source_length = *source_length_ptr;

  if (source != source_buffer) {
    if (source_length != 0) {
      memmove(source_buffer, source, source_length);
    }

    // Source can be accessed even if next code will fail.
    *source_ptr = source_buffer;
  }

  size_t remaining_source_buffer_length = source_buffer_length - source_length;
  if (remaining_source_buffer_length == 0) {
    // We want to read more data at once, than buffer has.
    return BRS_EXT_ERROR_NOT_ENOUGH_SOURCE_BUFFER;
  }

  brs_ext_byte_t* remaining_source_buffer = source_buffer + source_length;
  size_t          new_source_length;

  brs_ext_result_t ext_result =
    read_file(source_file, remaining_source_buffer, &new_source_length, remaining_source_buffer_length);

  if (ext_result != 0) {
    return ext_result;
  }

  *source_length_ptr = source_length + new_source_length;

  return 0;
}

#define BUFFERED_READ_SOURCE(function, ...)                                                                     \
  do {                                                                                                          \
    bool is_function_called = false;                                                                            \
                                                                                                                \
    while (true) {                                                                                              \
      ext_result = read_more_source(source_file, &source, &source_length, source_buffer, source_buffer_length); \
      if (ext_result == BRS_EXT_FILE_READ_FINISHED) {                                                           \
        if (source_length != 0) {                                                                               \
          /* Brotli won't provide any remainder by design. */                                                   \
          return BRS_EXT_ERROR_READ_IO;                                                                         \
        }                                                                                                       \
        break;                                                                                                  \
      } else if (ext_result != 0) {                                                                             \
        return ext_result;                                                                                      \
      }                                                                                                         \
                                                                                                                \
      ext_result = function(__VA_ARGS__);                                                                       \
      if (ext_result != 0) {                                                                                    \
        return ext_result;                                                                                      \
      }                                                                                                         \
                                                                                                                \
      is_function_called = true;                                                                                \
    }                                                                                                           \
                                                                                                                \
    if (!is_function_called) {                                                                                  \
      /* Function should be called at least once. */                                                            \
      ext_result = function(__VA_ARGS__);                                                                       \
      if (ext_result != 0) {                                                                                    \
        return ext_result;                                                                                      \
      }                                                                                                         \
    }                                                                                                           \
  } while (false);

// Algorithm has written data into destination buffer.
// We need to write this data into file.
// Than algorithm can use same buffer again.

static inline brs_ext_result_t flush_destination_buffer(
  FILE*           destination_file,
  brs_ext_byte_t* destination_buffer,
  size_t*         destination_length_ptr,
  size_t          destination_buffer_length)
{
  if (*destination_length_ptr == 0) {
    // We want to write more data at once, than buffer has.
    return BRS_EXT_ERROR_NOT_ENOUGH_DESTINATION_BUFFER;
  }

  brs_ext_result_t ext_result = write_file(destination_file, destination_buffer, *destination_length_ptr);
  if (ext_result != 0) {
    return ext_result;
  }

  *destination_length_ptr = 0;

  return 0;
}

static inline brs_ext_result_t
  write_remaining_destination(FILE* destination_file, brs_ext_byte_t* destination_buffer, size_t destination_length)
{
  if (destination_length == 0) {
    return 0;
  }

  return write_file(destination_file, destination_buffer, destination_length);
}

// -- utils --

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

// -- buffered compress --

typedef struct
{
  BrotliEncoderState*    state_ptr;
  const brs_ext_byte_t** source_ptr;
  size_t*                source_length_ptr;
  brs_ext_byte_t*        remaining_destination_buffer;
  size_t*                remaining_destination_buffer_length_ptr;
  BROTLI_BOOL            result;
} compress_args_t;

static inline void* compress_wrapper(void* data)
{
  compress_args_t* args = data;

  args->result = BrotliEncoderCompressStream(
    args->state_ptr,
    BROTLI_OPERATION_PROCESS,
    args->source_length_ptr,
    args->source_ptr,
    args->remaining_destination_buffer_length_ptr,
    &args->remaining_destination_buffer,
    NULL);

  return NULL;
}

static inline brs_ext_result_t buffered_compress(
  BrotliEncoderState*    state_ptr,
  const brs_ext_byte_t** source_ptr,
  size_t*                source_length_ptr,
  FILE*                  destination_file,
  brs_ext_byte_t*        destination_buffer,
  size_t*                destination_length_ptr,
  size_t                 destination_buffer_length,
  bool                   gvl)
{
  brs_ext_result_t ext_result;
  compress_args_t  args = {.state_ptr = state_ptr, .source_ptr = source_ptr, .source_length_ptr = source_length_ptr};

  while (true) {
    brs_ext_byte_t* remaining_destination_buffer             = destination_buffer + *destination_length_ptr;
    size_t          remaining_destination_buffer_length      = destination_buffer_length - *destination_length_ptr;
    size_t          prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    args.remaining_destination_buffer            = remaining_destination_buffer;
    args.remaining_destination_buffer_length_ptr = &remaining_destination_buffer_length;

    BRS_EXT_GVL_WRAP(gvl, compress_wrapper, &args);
    if (!args.result) {
      return BRS_EXT_ERROR_UNEXPECTED;
    }

    *destination_length_ptr += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (BrotliEncoderHasMoreOutput(state_ptr)) {
      ext_result = flush_destination_buffer(
        destination_file, destination_buffer, destination_length_ptr, destination_buffer_length);

      if (ext_result != 0) {
        return ext_result;
      }

      continue;
    }

    break;
  }

  return 0;
}

// -- buffered compressor finish --

typedef struct
{
  BrotliEncoderState*    state_ptr;
  const brs_ext_byte_t** source_ptr;
  size_t*                source_length_ptr;
  brs_ext_byte_t*        remaining_destination_buffer;
  size_t*                remaining_destination_buffer_length_ptr;
  BROTLI_BOOL            result;
} compressor_finish_args_t;

static inline void* compressor_finish_wrapper(void* data)
{
  compressor_finish_args_t* args = data;

  args->result = BrotliEncoderCompressStream(
    args->state_ptr,
    BROTLI_OPERATION_FINISH,
    args->source_length_ptr,
    args->source_ptr,
    args->remaining_destination_buffer_length_ptr,
    &args->remaining_destination_buffer,
    NULL);

  return NULL;
}

static inline brs_ext_result_t buffered_compressor_finish(
  BrotliEncoderState* state_ptr,
  FILE*               destination_file,
  brs_ext_byte_t*     destination_buffer,
  size_t*             destination_length_ptr,
  size_t              destination_buffer_length,
  bool                gvl)
{
  brs_ext_result_t         ext_result;
  const brs_ext_byte_t*    source        = NULL;
  size_t                   source_length = 0;
  compressor_finish_args_t args = {.state_ptr = state_ptr, .source_ptr = &source, .source_length_ptr = &source_length};

  while (true) {
    brs_ext_byte_t* remaining_destination_buffer             = destination_buffer + *destination_length_ptr;
    size_t          remaining_destination_buffer_length      = destination_buffer_length - *destination_length_ptr;
    size_t          prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    args.remaining_destination_buffer            = remaining_destination_buffer;
    args.remaining_destination_buffer_length_ptr = &remaining_destination_buffer_length;

    BRS_EXT_GVL_WRAP(gvl, compressor_finish_wrapper, &args);
    if (!args.result) {
      return BRS_EXT_ERROR_UNEXPECTED;
    }

    *destination_length_ptr += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (BrotliEncoderHasMoreOutput(state_ptr) || !BrotliEncoderIsFinished(state_ptr)) {
      ext_result = flush_destination_buffer(
        destination_file, destination_buffer, destination_length_ptr, destination_buffer_length);

      if (ext_result != 0) {
        return ext_result;
      }

      continue;
    }

    break;
  }

  return 0;
}

// -- compress --

static inline brs_ext_result_t compress(
  BrotliEncoderState* state_ptr,
  FILE*               source_file,
  brs_ext_byte_t*     source_buffer,
  size_t              source_buffer_length,
  FILE*               destination_file,
  brs_ext_byte_t*     destination_buffer,
  size_t              destination_buffer_length,
  bool                gvl)
{
  brs_ext_result_t      ext_result;
  const brs_ext_byte_t* source             = source_buffer;
  size_t                source_length      = 0;
  size_t                destination_length = 0;

  BUFFERED_READ_SOURCE(
    buffered_compress,
    state_ptr,
    &source,
    &source_length,
    destination_file,
    destination_buffer,
    &destination_length,
    destination_buffer_length,
    gvl);

  ext_result = buffered_compressor_finish(
    state_ptr, destination_file, destination_buffer, &destination_length, destination_buffer_length, gvl);

  if (ext_result != 0) {
    return ext_result;
  }

  return write_remaining_destination(destination_file, destination_buffer, destination_length);
}

VALUE brs_ext_compress_io(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE destination, VALUE options)
{
  GET_FILE(source);
  GET_FILE(destination);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_SIZE_OPTION(options, source_buffer_length);
  BRS_EXT_GET_SIZE_OPTION(options, destination_buffer_length);
  BRS_EXT_GET_BOOL_OPTION(options, gvl);
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

  if (source_buffer_length == 0) {
    source_buffer_length = BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_COMPRESSOR;
  }
  if (destination_buffer_length == 0) {
    destination_buffer_length = BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_COMPRESSOR;
  }

  brs_ext_byte_t* source_buffer;
  brs_ext_byte_t* destination_buffer;

  ext_result = create_buffers(&source_buffer, source_buffer_length, &destination_buffer, destination_buffer_length);
  if (ext_result != 0) {
    BrotliEncoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  ext_result = compress(
    state_ptr,
    source_file,
    source_buffer,
    source_buffer_length,
    destination_file,
    destination_buffer,
    destination_buffer_length,
    gvl);

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

// -- buffered decompress --

typedef struct
{
  BrotliDecoderState*    state_ptr;
  const brs_ext_byte_t** source_ptr;
  size_t*                source_length_ptr;
  brs_ext_byte_t*        remaining_destination_buffer;
  size_t*                remaining_destination_buffer_length_ptr;
  BrotliDecoderResult    result;
} decompress_args_t;

static inline void* decompress_wrapper(void* data)
{
  decompress_args_t* args = data;

  args->result = BrotliDecoderDecompressStream(
    args->state_ptr,
    args->source_length_ptr,
    args->source_ptr,
    args->remaining_destination_buffer_length_ptr,
    &args->remaining_destination_buffer,
    NULL);

  return NULL;
}

static inline brs_ext_result_t buffered_decompress(
  BrotliDecoderState*    state_ptr,
  const brs_ext_byte_t** source_ptr,
  size_t*                source_length_ptr,
  FILE*                  destination_file,
  brs_ext_byte_t*        destination_buffer,
  size_t*                destination_length_ptr,
  size_t                 destination_buffer_length,
  bool                   gvl)
{
  brs_ext_result_t  ext_result;
  decompress_args_t args = {.state_ptr = state_ptr, .source_ptr = source_ptr, .source_length_ptr = source_length_ptr};

  while (true) {
    brs_ext_byte_t* remaining_destination_buffer             = destination_buffer + *destination_length_ptr;
    size_t          remaining_destination_buffer_length      = destination_buffer_length - *destination_length_ptr;
    size_t          prev_remaining_destination_buffer_length = remaining_destination_buffer_length;

    args.remaining_destination_buffer            = remaining_destination_buffer;
    args.remaining_destination_buffer_length_ptr = &remaining_destination_buffer_length;

    BRS_EXT_GVL_WRAP(gvl, decompress_wrapper, &args);

    if (
      args.result != BROTLI_DECODER_RESULT_SUCCESS && args.result != BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT &&
      args.result != BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      BrotliDecoderErrorCode error_code = BrotliDecoderGetErrorCode(state_ptr);
      return brs_ext_get_decompressor_error(error_code);
    }

    *destination_length_ptr += prev_remaining_destination_buffer_length - remaining_destination_buffer_length;

    if (args.result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
      ext_result = flush_destination_buffer(
        destination_file, destination_buffer, destination_length_ptr, destination_buffer_length);

      if (ext_result != 0) {
        return ext_result;
      }

      continue;
    }

    break;
  }

  return 0;
}

// -- decompress --

static inline brs_ext_result_t decompress(
  BrotliDecoderState* state_ptr,
  FILE*               source_file,
  brs_ext_byte_t*     source_buffer,
  size_t              source_buffer_length,
  FILE*               destination_file,
  brs_ext_byte_t*     destination_buffer,
  size_t              destination_buffer_length,
  bool                gvl)
{
  brs_ext_result_t      ext_result;
  const brs_ext_byte_t* source             = source_buffer;
  size_t                source_length      = 0;
  size_t                destination_length = 0;

  BUFFERED_READ_SOURCE(
    buffered_decompress,
    state_ptr,
    &source,
    &source_length,
    destination_file,
    destination_buffer,
    &destination_length,
    destination_buffer_length,
    gvl);

  return write_remaining_destination(destination_file, destination_buffer, destination_length);
}

VALUE brs_ext_decompress_io(VALUE BRS_EXT_UNUSED(self), VALUE source, VALUE destination, VALUE options)
{
  GET_FILE(source);
  GET_FILE(destination);
  Check_Type(options, T_HASH);
  BRS_EXT_GET_SIZE_OPTION(options, source_buffer_length);
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

  if (source_buffer_length == 0) {
    source_buffer_length = BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_DECOMPRESSOR;
  }
  if (destination_buffer_length == 0) {
    destination_buffer_length = BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_DECOMPRESSOR;
  }

  brs_ext_byte_t* source_buffer;
  brs_ext_byte_t* destination_buffer;

  ext_result = create_buffers(&source_buffer, source_buffer_length, &destination_buffer, destination_buffer_length);
  if (ext_result != 0) {
    BrotliDecoderDestroyInstance(state_ptr);
    brs_ext_raise_error(ext_result);
  }

  ext_result = decompress(
    state_ptr,
    source_file,
    source_buffer,
    source_buffer_length,
    destination_file,
    destination_buffer,
    destination_buffer_length,
    gvl);

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

// -- exports --

void brs_ext_io_exports(VALUE root_module)
{
  rb_define_module_function(root_module, "_native_compress_io", RUBY_METHOD_FUNC(brs_ext_compress_io), 3);
  rb_define_module_function(root_module, "_native_decompress_io", RUBY_METHOD_FUNC(brs_ext_decompress_io), 3);
}
