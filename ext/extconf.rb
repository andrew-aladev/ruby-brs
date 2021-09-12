# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "mkmf"

have_func "rb_thread_call_without_gvl", "ruby/thread.h"

def require_header(name, constants: [], macroses: [], types: [])
  abort "Can't find #{name} header" unless find_header name

  constants.each do |constant|
    abort "Can't find #{constant} constant in #{name} header" unless have_const constant, name
  end

  macroses.each do |macro|
    abort "Can't find #{macro} macro in #{name} header" unless have_macro macro, name
  end

  types.each do |type|
    abort "Can't find #{type} type in #{name} header" unless find_type type, nil, name
  end
end

require_header(
  "brotli/decode.h",
  :constants => %w[
    BROTLI_DECODER_ERROR_ALLOC_BLOCK_TYPE_TREES
    BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MAP
    BROTLI_DECODER_ERROR_ALLOC_CONTEXT_MODES
    BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_1
    BROTLI_DECODER_ERROR_ALLOC_RING_BUFFER_2
    BROTLI_DECODER_ERROR_ALLOC_TREE_GROUPS
    BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_1
    BROTLI_DECODER_ERROR_FORMAT_BLOCK_LENGTH_2
    BROTLI_DECODER_ERROR_FORMAT_CL_SPACE
    BROTLI_DECODER_ERROR_FORMAT_CONTEXT_MAP_REPEAT
    BROTLI_DECODER_ERROR_FORMAT_DICTIONARY
    BROTLI_DECODER_ERROR_FORMAT_DISTANCE
    BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_META_NIBBLE
    BROTLI_DECODER_ERROR_FORMAT_EXUBERANT_NIBBLE
    BROTLI_DECODER_ERROR_FORMAT_HUFFMAN_SPACE
    BROTLI_DECODER_ERROR_FORMAT_PADDING_1
    BROTLI_DECODER_ERROR_FORMAT_PADDING_2
    BROTLI_DECODER_ERROR_FORMAT_RESERVED
    BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_ALPHABET
    BROTLI_DECODER_ERROR_FORMAT_SIMPLE_HUFFMAN_SAME
    BROTLI_DECODER_ERROR_FORMAT_TRANSFORM
    BROTLI_DECODER_ERROR_FORMAT_WINDOW_BITS
    BROTLI_DECODER_PARAM_DISABLE_RING_BUFFER_REALLOCATION
    BROTLI_DECODER_PARAM_LARGE_WINDOW
    BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT
    BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT
    BROTLI_DECODER_RESULT_SUCCESS
  ],
  :types     => [
    "BrotliDecoderErrorCode",
    "BrotliDecoderResult",
    "BrotliDecoderState *"
  ]
)
require_header(
  "brotli/encode.h",
  :constants => %w[
    BROTLI_MAX_INPUT_BLOCK_BITS
    BROTLI_MAX_QUALITY
    BROTLI_MAX_WINDOW_BITS
    BROTLI_MIN_INPUT_BLOCK_BITS
    BROTLI_MIN_QUALITY
    BROTLI_MIN_WINDOW_BITS
    BROTLI_MODE_FONT
    BROTLI_MODE_GENERIC
    BROTLI_MODE_TEXT
    BROTLI_OPERATION_FINISH
    BROTLI_OPERATION_FLUSH
    BROTLI_OPERATION_PROCESS
    BROTLI_PARAM_DISABLE_LITERAL_CONTEXT_MODELING
    BROTLI_PARAM_LARGE_WINDOW
    BROTLI_PARAM_LGBLOCK
    BROTLI_PARAM_LGWIN
    BROTLI_PARAM_NDIRECT
    BROTLI_PARAM_NPOSTFIX
    BROTLI_PARAM_MODE
    BROTLI_PARAM_QUALITY
    BROTLI_PARAM_SIZE_HINT
  ],
  :types     => [
    "BrotliEncoderMode",
    "BrotliEncoderState *"
  ]
)
require_header(
  "brotli/types.h",
  :macroses => %w[BROTLI_BOOL]
)

def require_library(name, functions)
  functions.each do |function|
    abort "Can't find #{function} function in #{name} library" unless find_library name, function
  end
end

require_library(
  "brotlienc",
  %w[
    BrotliEncoderCompressStream
    BrotliEncoderCreateInstance
    BrotliEncoderDestroyInstance
    BrotliEncoderHasMoreOutput
    BrotliEncoderIsFinished
    BrotliEncoderSetParameter
    BrotliEncoderVersion
  ]
)

require_library(
  "brotlidec",
  %w[
    BrotliDecoderCreateInstance
    BrotliDecoderDecompressStream
    BrotliDecoderDestroyInstance
    BrotliDecoderGetErrorCode
    BrotliDecoderSetParameter
    BrotliDecoderVersion
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
