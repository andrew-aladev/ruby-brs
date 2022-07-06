# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/stream/writer_helpers"
require "brs/stream/writer"
require "brs/string"

require_relative "../minitest"
require_relative "../option"

module BRS
  module Test
    module Stream
      class WriterHelpers < ADSP::Test::Stream::WriterHelpers
        Target = BRS::Stream::Writer
        Option = Test::Option
        String = BRS::String
      end

      Minitest << WriterHelpers
    end
  end
end
