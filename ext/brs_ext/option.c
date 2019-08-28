// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/error.h"
#include "brs_ext/option.h"

static inline VALUE get_option(VALUE options, const char *name)
{
  return rb_funcall(options, rb_intern("[]"), 1, ID2SYM(rb_intern(name)));
}

unsigned long brs_ext_get_required_fixnum_option(VALUE options, const char *name)
{
  VALUE value = get_option(options, name);

  Check_Type(value, T_FIXNUM);

  return rb_num2uint(value);
}

uint32_t *brs_ext_get_mode_option(VALUE options)
{
  VALUE     value  = get_option(options, "mode");
  uint32_t *option = NULL;

  if (value != Qnil) {
    Check_Type(value, T_SYMBOL);

    ID id = SYM2ID(value);
    if (id == rb_intern("text")) {
      *option = BROTLI_MODE_TEXT;
    }
    else if (id == rb_intern("font")) {
      *option = BROTLI_MODE_FONT;
    }
    else if (id == rb_intern("generic")) {
      *option = BROTLI_MODE_GENERIC;
    }
    else {
      brs_ext_raise_error("ValidateError", "invalid mode value");
    }
  }

  return option;
}

uint32_t *brs_ext_get_fixnum_option(VALUE options, const char *name)
{
  VALUE     value  = get_option(options, name);
  uint32_t *option = NULL;

  if (value != Qnil) {
    Check_Type(value, T_FIXNUM);

    *option = rb_num2uint(value);
  }

  return option;
}

uint32_t *brs_ext_get_bool_option(VALUE options, const char *name)
{
  VALUE     value  = get_option(options, name);
  uint32_t *option = NULL;

  if (value != Qnil) {
    int type = TYPE(value);
    if (type != T_TRUE && type != T_FALSE) {
      brs_ext_raise_error("ValidateError", "invalid bool value");
    }

    *option = type == T_TRUE ? 1 : 0;
  }

  return option;
}
