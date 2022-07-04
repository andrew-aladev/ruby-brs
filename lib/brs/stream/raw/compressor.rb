# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/stream/raw/compressor"
require "brs_ext"

require_relative "../../option"
require_relative "../../validation"

module BRS
  module Stream
    module Raw
      # BRS::Stream::Raw::Compressor class.
      class Compressor < ADSP::Stream::Raw::Compressor
        # Current native compressor class.
        NativeCompressor = Stream::NativeCompressor

        # Current option class.
        Option = BRS::Option

        # Initializes compressor.
        # Option: +:destination_buffer_length+ destination buffer length.
        # Option: +:size_hint+ source bytesize.
        def initialize(options = {})
          options = Option.get_compressor_options options, BUFFER_LENGTH_NAMES

          size_hint = options[:size_hint]
          Validation.validate_not_negative_integer size_hint unless size_hint.nil?

          super options
        end
      end
    end
  end
end
