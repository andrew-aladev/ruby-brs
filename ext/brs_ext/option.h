// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_OPTIONS_H)
#define BRS_EXT_OPTIONS_H

#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/error.h"

// All options has uint32_t type.
// Option should not be set when pointer to option value is NULL.

#define BRS_EXT_GET_OPTION(name)                                                              \
  VALUE     name##_value = rb_funcall(options, rb_intern("[]"), 1, ID2SYM(rb_intern(#name))); \
  uint32_t *name         = NULL;

#define BRS_EXT_GET_BOOL_OPTION(name)                             \
  BRS_EXT_GET_OPTION(name);                                       \
                                                                  \
  if (name##_value != Qnil) {                                     \
    int name##_type = TYPE(name##_value);                         \
    if (name##_type != T_TRUE && name##_type != T_FALSE) {        \
      brs_ext_raise_error("ValidateError", "invalid bool value"); \
    }                                                             \
                                                                  \
    *name = name##_type == T_TRUE ? 1 : 0;                        \
  }

#define BRS_EXT_GET_FIXNUM_OPTION(name) \
  BRS_EXT_GET_OPTION(name);             \
                                        \
  if (name##_value != Qnil) {           \
    Check_Type(name##_value, T_FIXNUM); \
                                        \
    *name = rb_num2uint(name##_value);  \
  }

#define BRS_EXT_GET_MODE_OPTION                                   \
  BRS_EXT_GET_OPTION(mode);                                       \
                                                                  \
  if (mode_value != Qnil) {                                       \
    Check_Type(mode_value, T_SYMBOL);                             \
                                                                  \
    ID mode_id = SYM2ID(mode_value);                              \
    if (mode_id == rb_intern("text")) {                           \
      *mode = BROTLI_MODE_TEXT;                                   \
    }                                                             \
    else if (mode_id == rb_intern("font")) {                      \
      *mode = BROTLI_MODE_FONT;                                   \
    }                                                             \
    else if (mode_id == rb_intern("generic")) {                   \
      *mode = BROTLI_MODE_GENERIC;                                \
    }                                                             \
    else {                                                        \
      brs_ext_raise_error("ValidateError", "invalid mode value"); \
    }                                                             \
  }

#define BRS_EXT_GET_COMPRESSOR_OPTIONS(options)              \
  Check_Type(options, T_HASH);                               \
                                                             \
  BRS_EXT_GET_MODE_OPTION();                                 \
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
