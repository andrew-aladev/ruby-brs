# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/stream/raw/decompressor"
require "brs/stream/raw/decompressor"
require "brs/string"

require_relative "../../minitest"
require_relative "../../option"

module BRS
  module Test
    module Stream
      module Raw
        class Decompressor < ADSP::Test::Stream::Raw::Decompressor
          Target = BRS::Stream::Raw::Decompressor
          Option = Test::Option
          String = BRS::String
        end

        Minitest << Decompressor
      end
    end
  end
end
