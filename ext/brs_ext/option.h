// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_OPTIONS_H)
#define BRS_EXT_OPTIONS_H

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "brs_ext/common.h"
#include "ruby.h"

// Default option values depends on brotli library.
// We will set only user defined values.

enum {
  BRS_EXT_OPTION_TYPE_BOOL = 1,
  BRS_EXT_OPTION_TYPE_UINT,
  BRS_EXT_OPTION_TYPE_MODE
};

typedef uint_fast8_t brs_ext_option_type_t;
typedef uint32_t     brs_ext_option_value_t;

typedef struct {
  bool                   has_value;
  brs_ext_option_value_t value;
} brs_ext_option_t;

typedef struct {
  brs_ext_option_t mode;
  brs_ext_option_t quality;
  brs_ext_option_t lgwin;
  brs_ext_option_t lgblock;
  brs_ext_option_t disable_literal_context_modeling;
  brs_ext_option_t size_hint;
  brs_ext_option_t large_window;
} brs_ext_compressor_options_t;

typedef struct {
  brs_ext_option_t disable_ring_buffer_reallocation;
  brs_ext_option_t large_window;
} brs_ext_decompressor_options_t;

void brs_ext_get_option(VALUE options, brs_ext_option_t* option, brs_ext_option_type_t type, const char* name);

#define BRS_EXT_GET_OPTION(options, target_options, type, name) \
  brs_ext_get_option(options, &target_options.name, type, #name);

#define BRS_EXT_GET_COMPRESSOR_OPTIONS(options)                                                                \
  brs_ext_compressor_options_t compressor_options;                                                             \
                                                                                                               \
  BRS_EXT_GET_OPTION(options, compressor_options, BRS_EXT_OPTION_TYPE_MODE, mode);                             \
  BRS_EXT_GET_OPTION(options, compressor_options, BRS_EXT_OPTION_TYPE_UINT, quality);                          \
  BRS_EXT_GET_OPTION(options, compressor_options, BRS_EXT_OPTION_TYPE_UINT, lgwin);                            \
  BRS_EXT_GET_OPTION(options, compressor_options, BRS_EXT_OPTION_TYPE_UINT, lgblock);                          \
  BRS_EXT_GET_OPTION(options, compressor_options, BRS_EXT_OPTION_TYPE_BOOL, disable_literal_context_modeling); \
  BRS_EXT_GET_OPTION(options, compressor_options, BRS_EXT_OPTION_TYPE_UINT, size_hint);                        \
  BRS_EXT_GET_OPTION(options, compressor_options, BRS_EXT_OPTION_TYPE_BOOL, large_window);

#define BRS_EXT_GET_DECOMPRESSOR_OPTIONS(options)                                                                \
  brs_ext_decompressor_options_t decompressor_options;                                                           \
                                                                                                                 \
  BRS_EXT_GET_OPTION(options, decompressor_options, BRS_EXT_OPTION_TYPE_BOOL, disable_ring_buffer_reallocation); \
  BRS_EXT_GET_OPTION(options, decompressor_options, BRS_EXT_OPTION_TYPE_BOOL, large_window);

size_t brs_ext_get_size_option_value(VALUE options, const char* name);

#define BRS_EXT_GET_BUFFER_LENGTH_OPTION(options, name) \
  size_t name = brs_ext_get_size_option_value(options, #name);

brs_ext_result_t brs_ext_set_compressor_options(BrotliEncoderState* state_ptr, brs_ext_compressor_options_t* options);
brs_ext_result_t brs_ext_set_decompressor_options(BrotliDecoderState* state_ptr, brs_ext_decompressor_options_t* options);

void brs_ext_option_exports(VALUE root_module);

#endif // BRS_EXT_OPTIONS_H
