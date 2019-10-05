// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#if !defined(BRS_EXT_IO_H)
#define BRS_EXT_IO_H

#include "ruby.h"

VALUE brs_ext_compress_io(VALUE self, VALUE source, VALUE destination, VALUE options);
VALUE brs_ext_decompress_io(VALUE self, VALUE source, VALUE destination, VALUE options);

void brs_ext_io_exports(VALUE root_module);

#endif // BRS_EXT_IO_H
