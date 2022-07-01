# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/stream/writer"

require_relative "raw/compressor"

module BRS
  module Stream
    # BRS::Stream::Writer class.
    class Writer < ADSP::Stream::Writer
      # Current raw stream class.
      RawCompressor = Raw::Compressor
    end
  end
end
