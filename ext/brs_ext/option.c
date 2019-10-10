// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/option.h"

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <stdbool.h>

#include "brs_ext/error.h"
#include "ruby.h"

static inline VALUE get_option(VALUE options, const char* name)
{
  return rb_funcall(options, rb_intern("[]"), 1, ID2SYM(rb_intern(name)));
}

bool brs_ext_get_bool_option(VALUE options, const char* name)
{
  VALUE value = get_option(options, name);

  int type = TYPE(value);
  if (type != T_TRUE && type != T_FALSE) {
    brs_ext_raise_error(BRS_EXT_ERROR_VALIDATE_FAILED);
  }

  return type == T_TRUE;
}

unsigned long brs_ext_get_fixnum_option(VALUE options, const char* name)
{
  VALUE value = get_option(options, name);

  Check_Type(value, T_FIXNUM);

  return NUM2UINT(value);
}

unsigned long brs_ext_get_mode_option(VALUE options, const char* name)
{
  VALUE value = get_option(options, name);

  Check_Type(value, T_SYMBOL);

  ID id = SYM2ID(value);
  if (id == rb_intern("text")) {
    return BROTLI_MODE_TEXT;
  }
  else if (id == rb_intern("font")) {
    return BROTLI_MODE_FONT;
  }
  else if (id == rb_intern("generic")) {
    return BROTLI_MODE_GENERIC;
  }
  else {
    brs_ext_raise_error(BRS_EXT_ERROR_VALIDATE_FAILED);
  }
}

static inline unsigned long serialize_option(VALUE options, brs_ext_option_t type, const char* name)
{
  switch (type) {
    case BRS_EXT_OPTION_TYPE_BOOL:
      return brs_ext_get_bool_option(options, name) ? 1 : 0;
    case BRS_EXT_OPTION_TYPE_FIXNUM:
      return brs_ext_get_fixnum_option(options, name);
    case BRS_EXT_OPTION_TYPE_MODE:
      return brs_ext_get_mode_option(options, name);
    default:
      brs_ext_raise_error(BRS_EXT_ERROR_UNEXPECTED);
  }
}

static inline bool has_option(VALUE options, const char* name)
{
  return rb_funcall(options, rb_intern("key?"), 1, ID2SYM(rb_intern(name))) == T_TRUE;
}

void brs_ext_set_compressor_option(
  BrotliEncoderState*    state_ptr,
  BrotliEncoderParameter param, VALUE options, brs_ext_option_t type, const char* name)
{
  if (has_option(options, name)) {
    unsigned long value = serialize_option(options, type, name);

    BROTLI_BOOL result = BrotliEncoderSetParameter(state_ptr, param, value);
    if (!result) {
      brs_ext_raise_error(BRS_EXT_ERROR_VALIDATE_FAILED);
    }
  }
}

void brs_ext_set_decompressor_option(
  BrotliDecoderState*    state_ptr,
  BrotliDecoderParameter param, VALUE options, brs_ext_option_t type, const char* name)
{
  if (has_option(options, name)) {
    unsigned long value = serialize_option(options, type, name);

    BROTLI_BOOL result = BrotliDecoderSetParameter(state_ptr, param, value);
    if (!result) {
      brs_ext_raise_error(BRS_EXT_ERROR_VALIDATE_FAILED);
    }
  }
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
