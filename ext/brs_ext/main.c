// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/buffer.h"
#include "brs_ext/io.h"
#include "brs_ext/option.h"
#include "brs_ext/stream/compressor.h"
#include "brs_ext/stream/decompressor.h"
#include "brs_ext/string.h"

void Init_brs_ext()
{
  VALUE root_module = rb_define_module(BRS_EXT_MODULE_NAME);

  brs_ext_buffer_exports(root_module);
  brs_ext_io_exports(root_module);
  brs_ext_option_exports(root_module);
  brs_ext_compressor_exports(root_module);
  brs_ext_decompressor_exports(root_module);
  brs_ext_string_exports(root_module);

  VALUE version_arguments[] = {INT2FIX(16)};
  VALUE version = rb_block_call(UINT2NUM(BrotliEncoderVersion()), rb_intern("to_s"), 1, version_arguments, 0, 0);
  rb_define_const(root_module, "LIBRARY_VERSION", rb_obj_freeze(version));
}
