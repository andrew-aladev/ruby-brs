# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "mkmf"

have_func "rb_thread_call_without_gvl", "ruby/thread.h"

def require_header(name, types = [])
  abort "Can't find #{name} header" unless find_header name

  types.each do |type|
    abort "Can't find #{type} type in #{name} header" unless find_type type, nil, name
  end
end

require_header "brotli/types.h", %w[BROTLI_BOOL]
require_header "brotli/encode.h", ["BrotliEncoderState *", "BrotliEncoderMode"]
require_header "brotli/decode.h", ["BrotliDecoderState *", "BrotliDecoderResult", "BrotliDecoderErrorCode"]

def require_library(name, functions)
  functions.each do |function|
    abort "Can't find #{function} function in #{name} library" unless find_library name, function
  end
end

require_library(
  "brotlienc",
  %w[
    BrotliEncoderCreateInstance
    BrotliEncoderSetParameter
    BrotliEncoderCompressStream
    BrotliEncoderHasMoreOutput
    BrotliEncoderIsFinished
    BrotliEncoderDestroyInstance
  ]
)

require_library(
  "brotlidec",
  %w[
    BrotliDecoderCreateInstance
    BrotliDecoderSetParameter
    BrotliDecoderDecompressStream
    BrotliDecoderGetErrorCode
    BrotliDecoderDestroyInstance
  ]
)

extension_name = "brs_ext".freeze
dir_config extension_name

# rubocop:disable Style/GlobalVars
$srcs = %w[
  stream/compressor
  stream/decompressor
  buffer
  error
  io
  main
  option
  string
]
.map { |name| "src/#{extension_name}/#{name}.c" }
.freeze

# Removing library duplicates.
$libs = $libs.split(%r{\s})
  .reject(&:empty?)
  .sort
  .uniq
  .join " "

if ENV["CI"]
  $CFLAGS << " --coverage"
  $LDFLAGS << " --coverage"
end

$CFLAGS << " -Wno-declaration-after-statement"

$VPATH << "$(srcdir)/#{extension_name}:$(srcdir)/#{extension_name}/stream"
# rubocop:enable Style/GlobalVars

create_makefile extension_name
