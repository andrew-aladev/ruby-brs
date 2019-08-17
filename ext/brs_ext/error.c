// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "ruby.h"

#include "brs_ext/common.h"
#include "brs_ext/error.h"

void brs_ext_raise_error(const char* name, const char* description)
{
  VALUE module = rb_define_module(BRS_EXT_MODULE_NAME);
  VALUE error  = rb_const_get(module, rb_intern(name));
  rb_raise(error, "%s", description);
}
