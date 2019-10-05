// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/common.h"
#include "brs_ext/io.h"
#include "brs_ext/option.h"
#include "brs_ext/stream/compressor.h"
#include "brs_ext/stream/decompressor.h"
#include "brs_ext/string.h"
#include "ruby.h"

void Init_brs_ext()
{
  VALUE root_module = rb_define_module(BRS_EXT_MODULE_NAME);

  brs_ext_option_exports(root_module);
  brs_ext_string_exports(root_module);
  brs_ext_io_exports(root_module);
  brs_ext_compressor_exports(root_module);
  brs_ext_decompressor_exports(root_module);
}
