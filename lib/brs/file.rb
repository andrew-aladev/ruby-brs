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

    # Compresses data from +source+ file path to +destination+ file path.
    # Option: +:source_buffer_length+ source buffer length.
    # Option: +:destination_buffer_length+ destination buffer length.
    # Option: +:size_hint+ source bytesize.
    def self.compress(source, destination, options = {})
      Validation.validate_string source

      options = Option.get_compressor_options options, BUFFER_LENGTH_NAMES

      options[:size_hint] = ::File.size source

      super source, destination, options
    end

    # Bypass native compress.
    def self.native_compress_io(*args)
      BRS._native_compress_io(*args)
    end

    # Bypass native decompress.
    def self.native_decompress_io(*args)
      BRS._native_decompress_io(*args)
    end
  end
end
