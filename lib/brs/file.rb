# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/file"
require "brs_ext"

require_relative "option"
require_relative "validation"

module BRS
  # BRS::File class.
  class File < ADSP::File
    # Current option class.
    Option = BRS::Option

    def self.compress(source, destination, options = {})
      Validation.validate_string source

      options = Option.get_compressor_options options, BUFFER_LENGTH_NAMES

      options[:size_hint] = ::File.size source

      super source, destination, options
    end

    def self.native_compress_io(*args)
      BRS._native_compress_io(*args)
    end

    def self.native_decompress_io(*args)
      BRS._native_decompress_io(*args)
    end
  end
end
