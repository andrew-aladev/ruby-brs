# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/stream/reader_helpers"
require "brs/stream/reader"
require "brs/string"

require_relative "../minitest"
require_relative "../option"

module BRS
  module Test
    module Stream
      class ReaderHelpers < ADSP::Test::Stream::ReaderHelpers
        Target = BRS::Stream::Reader
        Option = Test::Option
        String = BRS::String
      end

      Minitest << ReaderHelpers
    end
  end
end
