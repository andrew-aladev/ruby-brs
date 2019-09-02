// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include <brotli/decode.h>
#include <brotli/encode.h>

#include "ruby.h"

#include "brs_ext/common.h"
#include "brs_ext/stream/compressor.h"
#include "brs_ext/stream/decompressor.h"

void Init_brs_ext()
{
  VALUE root = rb_define_module(BRS_EXT_MODULE_NAME);

  // It is better to use these functions internally and prepare pretty wrappers for public usage.
  // rb_define_module_function(root, "_native_compress_io", RUBY_METHOD_FUNC(brs_ext_compress_io), 3);
  // rb_define_module_function(root, "_native_decompress_io", RUBY_METHOD_FUNC(brs_ext_decompress_io), 3);
  // rb_define_module_function(root, "_native_compress_string", RUBY_METHOD_FUNC(brs_ext_compress_string), 2);
  // rb_define_module_function(root, "_native_decompress_string", RUBY_METHOD_FUNC(brs_ext_decompress_string), 2);

  // -----

  VALUE stream = rb_define_module_under(root, "Stream");

  VALUE compressor = rb_define_class_under(stream, "NativeCompressor", rb_cObject);
  rb_define_alloc_func(compressor, brs_ext_allocate_compressor);
  rb_define_method(compressor, "initialize", brs_ext_initialize_compressor, 1);
  rb_define_method(compressor, "write", brs_ext_compress, 1);
  rb_define_method(compressor, "flush", brs_ext_flush_compressor, 0);
  rb_define_method(compressor, "finish", brs_ext_finish_compressor, 0);
  rb_define_method(compressor, "read_result", brs_ext_compressor_read_result, 0);
  rb_define_method(compressor, "close", brs_ext_compressor_close, 0);

  VALUE decompressor = rb_define_class_under(stream, "NativeDecompressor", rb_cObject);
  rb_define_alloc_func(decompressor, brs_ext_allocate_decompressor);
  rb_define_method(decompressor, "initialize", brs_ext_initialize_decompressor, 1);
  rb_define_method(decompressor, "read", brs_ext_decompress, 1);
  rb_define_method(decompressor, "read_result", brs_ext_decompressor_read_result, 0);
  rb_define_method(decompressor, "close", brs_ext_decompressor_close, 0);

  // -----

  VALUE option = rb_define_module_under(root, "Option");

  rb_define_const(
    option,
    "MODES",
    rb_ary_new_from_args(
      3,
      ID2SYM(rb_intern("text")),
      ID2SYM(rb_intern("font")),
      ID2SYM(rb_intern("generic"))
    )
  );

  rb_define_const(option, "MIN_QUALITY", INT2FIX(BROTLI_MIN_QUALITY));
  rb_define_const(option, "MAX_QUALITY", INT2FIX(BROTLI_MAX_QUALITY));

  rb_define_const(option, "MIN_LGWIN", INT2FIX(BROTLI_MIN_WINDOW_BITS));
  rb_define_const(option, "MAX_LGWIN", INT2FIX(BROTLI_MAX_WINDOW_BITS));

  rb_define_const(option, "MIN_LGBLOCK", INT2FIX(BROTLI_MIN_INPUT_BLOCK_BITS));
  rb_define_const(option, "MAX_LGBLOCK", INT2FIX(BROTLI_MAX_INPUT_BLOCK_BITS));
}
