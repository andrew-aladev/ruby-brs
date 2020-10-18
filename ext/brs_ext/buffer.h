// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_BUFFER_H)
#define BRS_EXT_BUFFER_H

#include "ruby.h"

#define BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_COMPRESSOR      (1 << 18) // 256 KB
#define BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_COMPRESSOR (1 << 16) // 64 KB

#define BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_DECOMPRESSOR      (1 << 16) // 64 KB
#define BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_DECOMPRESSOR (1 << 18) // 256 KB

VALUE brs_ext_create_string_buffer(VALUE length);

#define BRS_EXT_CREATE_STRING_BUFFER(buffer, length, exception) \
  VALUE buffer = rb_protect(brs_ext_create_string_buffer, SIZET2NUM(length), &exception);

VALUE brs_ext_resize_string_buffer(VALUE buffer_args);

#define BRS_EXT_RESIZE_STRING_BUFFER(buffer, length, exception)                          \
  VALUE buffer_args = rb_ary_new_from_args(2, buffer, SIZET2NUM(length));                \
  buffer            = rb_protect(brs_ext_resize_string_buffer, buffer_args, &exception); \
  RB_GC_GUARD(buffer_args);

void brs_ext_buffer_exports(VALUE root_module);

#endif // BRS_EXT_BUFFER_H
