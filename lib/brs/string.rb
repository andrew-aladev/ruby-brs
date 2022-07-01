# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/string"
require "brs_ext"

require_relative "option"
require_relative "validation"

module BRS
  # BRS::String class.
  class String < ADSP::String
    # Current option class.
    Option = BRS::Option

    def self.compress(source, options = {})
      Validation.validate_string source

      options = Option.get_compressor_options options, BUFFER_LENGTH_NAMES

      options[:size_hint] = source.bytesize

      super source, options
    end

    def self.native_compress_string(*args)
      BRS._native_compress_string(*args)
    end

    def self.native_decompress_string(*args)
      BRS._native_decompress_string(*args)
    end
  end
end
