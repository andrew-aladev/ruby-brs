# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "mkmf"

def require_header(name)
  abort "Can't find #{name} header" unless find_header name
end

require_header "brotli/encode.h"
require_header "brotli/decode.h"

def require_library(name, functions)
  functions.each do |function|
    abort "Can't find #{name} library and #{function} function" unless find_library name, function
  end
end

encoder_functions = %w[
  BrotliEncoderCreateInstance
  BrotliEncoderSetParameter
  BrotliEncoderCompressStream
  BrotliEncoderHasMoreOutput
  BrotliEncoderIsFinished
  BrotliEncoderDestroyInstance
]
.freeze

require_library "brotlienc", encoder_functions

decoder_functions = %w[
  BrotliDecoderCreateInstance
  BrotliDecoderSetParameter
  BrotliDecoderDecompressStream
  BrotliDecoderGetErrorCode
  BrotliDecoderDestroyInstance
]
.freeze

require_library "brotlidec", decoder_functions

extension_name = "brs_ext".freeze
dir_config extension_name

sources = %w[
  stream/compressor
  stream/decompressor
  error
  io
  main
  option
  string
]
.freeze

# rubocop:disable Style/GlobalVars
$srcs = sources
  .map { |name| "src/#{extension_name}/#{name}.c" }
  .freeze

$CFLAGS << " -Wno-declaration-after-statement"
$VPATH << "$(srcdir)/#{extension_name}:$(srcdir)/#{extension_name}/stream"
# rubocop:enable Style/GlobalVars

create_makefile extension_name
