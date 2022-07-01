# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/stream/raw/decompressor"
require "brs_ext"

require_relative "../../option"

module BRS
  module Stream
    module Raw
      # BRS::Stream::Raw::Decompressor class.
      class Decompressor < ADSP::Stream::Raw::Decompressor
        # Current native decompressor class.
        NativeDecompressor = Stream::NativeDecompressor

        # Current option class.
        Option = BRS::Option
      end
    end
  end
end
