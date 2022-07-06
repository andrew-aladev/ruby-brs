# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/stream/writer"
require "brs/stream/writer"
require "brs/string"

require_relative "../minitest"
require_relative "../option"

module BRS
  module Test
    module Stream
      class Writer < ADSP::Test::Stream::Writer
        Target = BRS::Stream::Writer
        Option = Test::Option
        String = BRS::String
      end

      Minitest << Writer
    end
  end
end
