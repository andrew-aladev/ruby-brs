// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_STRING_H)
#define BRS_EXT_STRING_H

#include "ruby.h"

VALUE brs_ext_compress_string(VALUE self, VALUE source, VALUE options);
VALUE brs_ext_decompress_string(VALUE self, VALUE source, VALUE options);

void brs_ext_string_exports(VALUE root_module);

#endif // BRS_EXT_STRING_H
