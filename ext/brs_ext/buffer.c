// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "ruby.h"

#include "brs_ext/buffer.h"

static inline uint8_t* allocate_buffer(size_t* buffer_length, size_t default_buffer_length)
{
  if (*buffer_length == 0) {
    *buffer_length = default_buffer_length;
  }

  return malloc(*buffer_length);
}

uint8_t* brs_ext_allocate_compressor_buffer(size_t* buffer_length)
{
  return allocate_buffer(buffer_length, DEFAULT_COMPRESSOR_BUFFER_LENGTH);
}

uint8_t* brs_ext_allocate_decompressor_buffer(size_t* buffer_length)
{
  return allocate_buffer(buffer_length, DEFAULT_DECOMPRESSOR_BUFFER_LENGTH);
}
