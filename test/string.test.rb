# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/string"

require_relative "common"
require_relative "minitest"
require_relative "option"
require_relative "validation"

module BRS
  module Test
    class String < Minitest::Unit::TestCase
      Target = BRS::String

      TEXTS = Common::TEXTS

      def test_invalid_arguments
        Validation::INVALID_STRINGS.each do |invalid_string|
          assert_raises ValidateError do
            Target.compress invalid_string
          end

          assert_raises ValidateError do
            Target.decompress invalid_string
          end
        end

        Option::INVALID_COMPRESSOR_OPTIONS.each do |invalid_options|
          assert_raises ValidateError do
            Target.compress "", invalid_options
          end
        end

        Option::INVALID_DECOMPRESSOR_OPTIONS.each do |invalid_options|
          assert_raises ValidateError do
            Target.decompress "", invalid_options
          end
        end
      end

      def test_invalid_text
        corrupted_compressed_text = Target.compress("1111").reverse

        assert_raises DecompressorCorruptedSourceError do
          Target.decompress corrupted_compressed_text
        end
      end

      def test_texts
        TEXTS.each do |text|
          Option::COMPRESSOR_OPTION_COMBINATIONS.each do |compressor_options|
            compressed_text = Target.compress text, compressor_options

            Option.get_compatible_decompressor_options(compressor_options) do |decompressor_options|
              decompressed_text = Target.decompress compressed_text, decompressor_options
              decompressed_text.force_encoding text.encoding

              assert_equal text, decompressed_text
            end
          end
        end
      end
    end

    Minitest << String
  end
end
