# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/stream/reader"
require "brs/stream/reader"
require "brs/string"
require "stringio"

require_relative "../minitest"
require_relative "../option"

module BRS
  module Test
    module Stream
      class Reader < ADSP::Test::Stream::Reader
        Target = BRS::Stream::Reader
        Option = Test::Option
        String = BRS::String

        def test_invalid_read
          super

          corrupted_compressed_text = String.compress("1111").reverse
          instance                  = Target.new ::StringIO.new(corrupted_compressed_text)

          assert_raises DecompressorCorruptedSourceError do
            instance.read
          end
        end

        def test_invalid_readpartial_and_read_nonblock
          super

          corrupted_compressed_text = String.compress("1111").reverse

          instance = Target.new ::StringIO.new(corrupted_compressed_text)

          assert_raises DecompressorCorruptedSourceError do
            instance.readpartial 1
          end

          instance = Target.new ::StringIO.new(corrupted_compressed_text)

          assert_raises DecompressorCorruptedSourceError do
            instance.read_nonblock 1
          end
        end
      end

      Minitest << Reader
    end
  end
end
