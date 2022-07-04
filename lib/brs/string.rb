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

    # Compresses +source+ string using +options+.
    # Option: +:destination_buffer_length+ destination buffer length.
    # Option: +:size_hint+ source bytesize.
    # Returns compressed string.
    def self.compress(source, options = {})
      Validation.validate_string source

      options = Option.get_compressor_options options, BUFFER_LENGTH_NAMES

      options[:size_hint] = source.bytesize

      super source, options
    end

    # Bypasses native compress.
    def self.native_compress_string(*args)
      BRS._native_compress_string(*args)
    end

    # Bypasses native decompress.
    def self.native_decompress_string(*args)
      BRS._native_decompress_string(*args)
    end
  end
end
