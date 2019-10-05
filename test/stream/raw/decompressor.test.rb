# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/stream/raw/decompressor"
require "brs/string"

require_relative "abstract"
require_relative "../../common"
require_relative "../../minitest"
require_relative "../../option"
require_relative "../../validation"

module BRS
  module Test
    module Stream
      module Raw
        class Decompressor < Abstract
          Target = BRS::Stream::Raw::Decompressor
          String = BRS::String

          TEXTS           = Common::TEXTS
          PORTION_LENGTHS = Common::PORTION_LENGTHS

          def test_invalid_initialize
            Option::INVALID_DECOMPRESSOR_OPTIONS.each do |invalid_options|
              assert_raises ValidateError do
                Target.new invalid_options
              end
            end
          end

          def test_invalid_read
            decompressor = Target.new

            Validation::INVALID_STRINGS.each do |invalid_string|
              assert_raises ValidateError do
                decompressor.read invalid_string, &NOOP_PROC
              end
            end

            assert_raises ValidateError do
              decompressor.read ""
            end

            corrupted_compressed_text = String.compress("1111").reverse

            assert_raises DecompressorCorruptedSourceError do
              decompressor.read corrupted_compressed_text, &NOOP_PROC
            end

            decompressor.close(&NOOP_PROC)

            assert_raises UsedAfterCloseError do
              decompressor.read "", &NOOP_PROC
            end
          end

          def test_texts
            TEXTS.each do |text|
              PORTION_LENGTHS.each do |portion_length|
                Option::COMPRESSOR_OPTION_COMBINATIONS.each do |compressor_options|
                  compressed_text = String.compress text, compressor_options

                  Option.get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                    decompressed_buffer = ::StringIO.new
                    decompressed_buffer.set_encoding ::Encoding::BINARY

                    writer = proc { |portion| decompressed_buffer << portion }

                    decompressor = Target.new decompressor_options

                    begin
                      source                 = "".b
                      compressed_text_offset = 0
                      index                  = 0

                      loop do
                        portion = compressed_text.byteslice compressed_text_offset, portion_length
                        break if portion.nil?

                        compressed_text_offset += portion_length
                        source << portion

                        bytes_read = decompressor.read source, &writer
                        source     = source.byteslice bytes_read, source.bytesize - bytes_read

                        decompressor.flush(&writer) if index.even?
                        index += 1
                      end

                    ensure
                      refute decompressor.closed?
                      decompressor.close(&writer)
                      assert decompressor.closed?
                    end

                    decompressed_text = decompressed_buffer.string
                    decompressed_text.force_encoding text.encoding

                    assert_equal text, decompressed_text
                  end
                end
              end
            end
          end
        end

        Minitest << Decompressor
      end
    end
  end
end
