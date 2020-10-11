// Ruby bindings for brotli library.
// Copyright (c) 2019 AUTHORS, MIT License.

#include "brs_ext/buffer.h"

#include "ruby.h"

VALUE brs_ext_create_string_buffer(VALUE length)
{
  return rb_str_new(NULL, NUM2SIZET(length));
}

VALUE brs_ext_resize_string_buffer(VALUE args)
{
  VALUE buffer = rb_ary_entry(args, 0);
  VALUE length = rb_ary_entry(args, 1);

  return rb_str_resize(buffer, NUM2SIZET(length));
}

void brs_ext_buffer_exports(VALUE root_module)
{
  VALUE module = rb_define_module_under(root_module, "Buffer");

  rb_define_const(
    module, "DEFAULT_SOURCE_BUFFER_LENGTH_FOR_COMPRESSOR", SIZET2NUM(BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_COMPRESSOR));

  rb_define_const(
    module,
    "DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_COMPRESSOR",
    SIZET2NUM(BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_COMPRESSOR));

  rb_define_const(
    module,
    "DEFAULT_SOURCE_BUFFER_LENGTH_FOR_DECOMPRESSOR",
    SIZET2NUM(BRS_DEFAULT_SOURCE_BUFFER_LENGTH_FOR_DECOMPRESSOR));

  rb_define_const(
    module,
    "DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_DECOMPRESSOR",
    SIZET2NUM(BRS_DEFAULT_DESTINATION_BUFFER_LENGTH_FOR_DECOMPRESSOR));
}
