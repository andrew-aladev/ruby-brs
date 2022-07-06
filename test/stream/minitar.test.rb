# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/stream/minitar"
require "brs/stream/reader"
require "brs/stream/writer"

require_relative "../minitest"

module BRS
  module Test
    module Stream
      class MinitarTest < ADSP::Test::Stream::MinitarTest
        Reader = BRS::Stream::Reader
        Writer = BRS::Stream::Writer
      end

      Minitest << MinitarTest
    end
  end
end
