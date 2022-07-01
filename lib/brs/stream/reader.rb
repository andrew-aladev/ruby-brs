# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/stream/reader"

require_relative "raw/decompressor"

module BRS
  module Stream
    # BRS::Stream::Reader class.
    class Reader < ADSP::Stream::Reader
      # Current raw stream class.
      RawDecompressor = Raw::Decompressor
    end
  end
end
