// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/option.h"

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <stdbool.h>

#include "brs_ext/common.h"
#include "brs_ext/error.h"
#include "ruby.h"

static inline VALUE get_raw_option_value(VALUE options, const char* name)
{
  return rb_funcall(options, rb_intern("[]"), 1, ID2SYM(rb_intern(name)));
}

static inline bool get_bool_option_value(VALUE raw_value)
{
  int raw_type = TYPE(raw_value);
  if (raw_type != T_TRUE && raw_type != T_FALSE) {
    brs_ext_raise_error(BRS_EXT_ERROR_VALIDATE_FAILED);
  }

  return raw_type == T_TRUE;
}

static inline unsigned long get_fixnum_option_value(VALUE raw_value)
{
  Check_Type(raw_value, T_FIXNUM);

  return NUM2UINT(raw_value);
}

static inline unsigned int get_mode_option_value(VALUE raw_value)
{
  Check_Type(raw_value, T_SYMBOL);

  ID raw_id = SYM2ID(raw_value);
  if (raw_id == rb_intern("text")) {
    return BROTLI_MODE_TEXT;
  }
  else if (raw_id == rb_intern("font")) {
    return BROTLI_MODE_FONT;
  }
  else if (raw_id == rb_intern("generic")) {
    return BROTLI_MODE_GENERIC;
  }
  else {
    brs_ext_raise_error(BRS_EXT_ERROR_VALIDATE_FAILED);
  }
}

void brs_ext_get_option(VALUE options, brs_ext_option_t* option, brs_ext_option_type_t type, const char* name)
{
  VALUE raw_value = get_raw_option_value(options, name);

  option->has_value = raw_value != Qnil;
  if (!option->has_value) {
    return;
  }

  brs_ext_option_value_t value;

  switch (type) {
    case BRS_EXT_OPTION_TYPE_BOOL:
      value = get_bool_option_value(raw_value) ? 1 : 0;
      break;
    case BRS_EXT_OPTION_TYPE_FIXNUM:
      value = get_fixnum_option_value(raw_value);
      break;
    case BRS_EXT_OPTION_TYPE_MODE:
      value = get_mode_option_value(raw_value);
      break;
    default:
      brs_ext_raise_error(BRS_EXT_ERROR_UNEXPECTED);
  }

  option->value = value;
}

unsigned long brs_ext_get_fixnum_option_value(VALUE options, const char* name)
{
  VALUE raw_value = get_raw_option_value(options, name);

  return get_fixnum_option_value(raw_value);
}

#define SET_OPTION_VALUE(function, state_ptr, param, option)           \
  if (option.has_value && !function(state_ptr, param, option.value)) { \
    return BRS_EXT_ERROR_VALIDATE_FAILED;                              \
  }

#define SET_ENCODER_PARAM(state_ptr, param, option) \
  SET_OPTION_VALUE(BrotliEncoderSetParameter, state_ptr, param, option);

brs_ext_result_t brs_ext_set_compressor_options(BrotliEncoderState* state_ptr, brs_ext_compressor_options_t* options)
{
  SET_ENCODER_PARAM(state_ptr, BROTLI_PARAM_MODE, options->mode);
  SET_ENCODER_PARAM(state_ptr, BROTLI_PARAM_QUALITY, options->quality);
  SET_ENCODER_PARAM(state_ptr, BROTLI_PARAM_LGWIN, options->lgwin);
  SET_ENCODER_PARAM(state_ptr, BROTLI_PARAM_LGBLOCK, options->lgblock);
  SET_ENCODER_PARAM(state_ptr, BROTLI_PARAM_DISABLE_LITERAL_CONTEXT_MODELING, options->disable_literal_context_modeling);
  SET_ENCODER_PARAM(state_ptr, BROTLI_PARAM_SIZE_HINT, options->size_hint);
  SET_ENCODER_PARAM(state_ptr, BROTLI_PARAM_LARGE_WINDOW, options->large_window);

  return 0;
}

#define SET_DECODER_PARAM(state_ptr, param, option) \
  SET_OPTION_VALUE(BrotliDecoderSetParameter, state_ptr, param, option);

brs_ext_result_t brs_ext_set_decompressor_options(BrotliDecoderState* state_ptr, brs_ext_decompressor_options_t* options)
{
  SET_DECODER_PARAM(state_ptr, BROTLI_DECODER_PARAM_DISABLE_RING_BUFFER_REALLOCATION, options->disable_ring_buffer_reallocation);
  SET_DECODER_PARAM(state_ptr, BROTLI_DECODER_PARAM_LARGE_WINDOW, options->large_window);

  return 0;
}

void brs_ext_option_exports(VALUE root_module)
{
  VALUE option = rb_define_module_under(root_module, "Option");

  VALUE modes = rb_ary_new_from_args(
    3,
    ID2SYM(rb_intern("text")),
    ID2SYM(rb_intern("font")),
    ID2SYM(rb_intern("generic")));
  rb_define_const(option, "MODES", modes);
  RB_GC_GUARD(modes);

  rb_define_const(option, "MIN_QUALITY", UINT2NUM(BROTLI_MIN_QUALITY));
  rb_define_const(option, "MAX_QUALITY", UINT2NUM(BROTLI_MAX_QUALITY));

  rb_define_const(option, "MIN_LGWIN", UINT2NUM(BROTLI_MIN_WINDOW_BITS));
  rb_define_const(option, "MAX_LGWIN", UINT2NUM(BROTLI_MAX_WINDOW_BITS));

  rb_define_const(option, "MIN_LGBLOCK", UINT2NUM(BROTLI_MIN_INPUT_BLOCK_BITS));
  rb_define_const(option, "MAX_LGBLOCK", UINT2NUM(BROTLI_MAX_INPUT_BLOCK_BITS));
}
