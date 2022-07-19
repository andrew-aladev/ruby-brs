# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/stream/raw/compressor"
require "brs/stream/raw/compressor"
require "brs/string"

require_relative "../../minitest"
require_relative "../../option"

module BRS
  module Test
    module Stream
      module Raw
        class Compressor < ADSP::Test::Stream::Raw::Compressor
          Target = BRS::Stream::Raw::Compressor
          Option = Test::Option
          String = BRS::String
        end

        Minitest << Compressor
      end
    end
  end
end
