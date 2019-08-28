// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_OPTIONS_H)
#define BRS_EXT_OPTIONS_H

#include "ruby.h"

unsigned long brs_ext_get_required_fixnum_option(VALUE options, const char *name);
uint32_t *    brs_ext_get_mode_option(VALUE options);
uint32_t *    brs_ext_get_fixnum_option(VALUE options, const char *name);
uint32_t *    brs_ext_get_bool_option(VALUE options, const char *name);

#define BRS_EXT_GET_REQUIRED_FIXNUM_OPTION(type, name) \
  type name = brs_ext_get_required_fixnum_option(options, #name);

#define BRS_EXT_GET_MODE_OPTION() \
  uint32_t *mode = brs_ext_get_mode_option(options);

#define BRS_EXT_GET_FIXNUM_OPTION(name) \
  uint32_t *name = brs_ext_get_fixnum_option(options, #name);

#define BRS_EXT_GET_BOOL_OPTION(name) \
  uint32_t *name = brs_ext_get_bool_option(options, #name);

#define BRS_EXT_GET_COMPRESSOR_OPTIONS(options)              \
  Check_Type(options, T_HASH);                               \
                                                             \
  BRS_EXT_GET_REQUIRED_FIXNUM_OPTION(size_t, buffer_length); \
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
  BRS_EXT_GET_REQUIRED_FIXNUM_OPTION(size_t, buffer_length); \
  BRS_EXT_GET_BOOL_OPTION(disable_ring_buffer_reallocation); \
  BRS_EXT_GET_BOOL_OPTION(large_window);

#endif // BRS_EXT_OPTIONS_H
