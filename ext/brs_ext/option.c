// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/decode.h>
#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/error.h"
#include "brs_ext/option.h"

static inline VALUE get_option(VALUE options, const char *name)
{
  return rb_funcall(options, rb_intern("[]"), 1, ID2SYM(rb_intern(name)));
}

// uint32_t *brs_ext_get_mode_option_ptr(VALUE options)
// {
//   VALUE     value      = get_option(options, "mode");
//   uint32_t *option_ptr = NULL;
//
//   if (value != Qnil) {
//     Check_Type(value, T_SYMBOL);
//
//     ID id = SYM2ID(value);
//     if (id == rb_intern("text")) {
//       *option_ptr = BROTLI_MODE_TEXT;
//     }
//     else if (id == rb_intern("font")) {
//       *option_ptr = BROTLI_MODE_FONT;
//     }
//     else if (id == rb_intern("generic")) {
//       *option_ptr = BROTLI_MODE_GENERIC;
//     }
//     else {
//       brs_ext_raise_error("ValidateError", "invalid mode value");
//     }
//   }
//
//   return option_ptr;
// }

// uint32_t *brs_ext_get_bool_option_ptr(VALUE options, const char *name)
// {
//   VALUE     value      = get_option(options, name);
//   uint32_t *option_ptr = NULL;
//
//   if (value != Qnil) {
//     int type = TYPE(value);
//     if (type != T_TRUE && type != T_FALSE) {
//       brs_ext_raise_error("ValidateError", "invalid bool value");
//     }
//
//     *option_ptr = type == T_TRUE ? 1 : 0;
//   }
//
//   return option_ptr;
// }

// uint32_t *brs_ext_get_fixnum_option_ptr(VALUE options, const char *name)
// {
//   VALUE     value      = get_option(options, name);
//   uint32_t *option_ptr = NULL;
//
//   if (value != Qnil) {
//     Check_Type(value, T_FIXNUM);
//
//     *option_ptr = rb_num2uint(value);
//   }
//
//   return option_ptr;
// }

void brs_ext_set_compressor_option(BrotliEncoderState *state_ptr, BrotliEncoderParameter param, brs_ext_option_t type, const char *name)
{
  // BROTLI_BOOL result = BrotliEncoderSetParameter(state_ptr, param_name, *option_ptr);
  //
  // if (!result) {
  //   brs_ext_raise_error("ValidateError", "invalid param value");
  // }
}

void brs_ext_set_decompressor_option(BrotliDecoderState *state_ptr, BrotliDecoderParameter param, brs_ext_option_t type, const char *name)
{
  // BROTLI_BOOL result = BrotliDecoderSetParameter(state_ptr, param_name, *option_ptr);
  //
  // if (!result) {
  //   brs_ext_raise_error("ValidateError", "invalid param value");
  // }
}

unsigned long brs_ext_get_fixnum_option(VALUE options, const char *name)
{
  VALUE value = get_option(options, name);

  Check_Type(value, T_FIXNUM);

  return rb_num2uint(value);
}
