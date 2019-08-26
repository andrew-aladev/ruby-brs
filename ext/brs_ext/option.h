// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_OPTIONS_H)
#define BRS_EXT_OPTIONS_H

#include <brotli/encode.h>

#include "ruby.h"

VALUE brs_ext_get_option(VALUE options, const char* name);
void  brs_ext_check_bool_type(VALUE option, const char* name);

#define BRS_EXT_GET_OPTION(name) \
  uint32_t* name##       = NULL; \
  VALUE     name##_value = brs_ext_get_option(options, "name##");

#define BRS_EXT_GET_BOOL_OPTION(name)                \
  BRS_EXT_GET_OPTION(name);                          \
                                                     \
  if (name##_value != Qnil) {                        \
    brs_ext_check_bool_type(name##_value, "name##"); \
    *name## = TYPE(name##_value) == T_TRUE ? 1 : 0;  \
  }

#define BRS_EXT_GET_FIXNUM_OPTION(name)  \
  BRS_EXT_GET_OPTION(uint32_t, name);    \
                                         \
  if (name##_value != Qnil) {            \
    Check_Type(name##_value, T_FIXNUM);  \
    *name## = rb_num2uint(name##_value); \
  }

#define BRS_EXT_GET_COMPRESSOR_OPTIONS(options)              \
  Check_Type(options, T_HASH);                               \
                                                             \
  BRS_EXT_GET_OPTION(mode);                                  \
                                                             \
  if (mode_value != Qnil) {                                  \
    Check_Type(mode_value, T_SYMBOL);                        \
                                                             \
    ID mode_id = SYM2ID(mode_value);                         \
    if (mode_id == rb_intern("text")) {                      \
      *mode = BROTLI_MODE_TEXT;                              \
    }                                                        \
    else if (mode_id == rb_intern("font")) {                 \
      *mode = BROTLI_MODE_FONT;                              \
    }                                                        \
    else {                                                   \
      *mode = BROTLI_MODE_GENERIC;                           \
    }                                                        \
  }                                                          \
                                                             \
  BRS_EXT_GET_FIXNUM_OPTION(quality);                        \
  BRS_EXT_GET_FIXNUM_OPTION(lgwin);                          \
  BRS_EXT_GET_FIXNUM_OPTION(lgblock);                        \
  BRS_EXT_GET_BOOL_OPTION(disable_literal_context_modeling); \
  BRS_EXT_GET_FIXNUM_OPTION(size_hint);                      \
  BRS_EXT_GET_BOOL_OPTION(large_window);                     \
  BRS_EXT_GET_FIXNUM_OPTION(npostfix);                       \
  BRS_EXT_GET_FIXNUM_OPTION(ndirect);

#define BRS_EXT_GET_DECOMPRESSOR_OPTIONS(options)            \
  Check_Type(options, T_HASH);                               \
                                                             \
  BRS_EXT_GET_BOOL_OPTION(disable_ring_buffer_reallocation); \
  BRS_EXT_GET_BOOL_OPTION(large_window);

#endif // BRS_EXT_OPTIONS_H
