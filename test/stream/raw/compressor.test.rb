# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/stream/raw/compressor"
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
        class Compressor < Abstract
          Target = BRS::Stream::Raw::Compressor
          String = BRS::String

          TEXTS           = Common::TEXTS
          PORTION_LENGTHS = Common::PORTION_LENGTHS

          def test_invalid_initialize
            Option::INVALID_COMPRESSOR_OPTIONS.each do |invalid_options|
              assert_raises ValidateError do
                Target.new invalid_options
              end
            end
          end

          def test_invalid_write
            compressor = Target.new

            Validation::INVALID_STRINGS.each do |invalid_string|
              assert_raises ValidateError do
                compressor.write invalid_string, &NOOP_PROC
              end
            end

            assert_raises ValidateError do
              compressor.write ""
            end

            compressor.close(&NOOP_PROC)

            assert_raises UsedAfterCloseError do
              compressor.write "", &NOOP_PROC
            end
          end

          def test_texts
            TEXTS.each do |text|
              PORTION_LENGTHS.each do |portion_length|
                Option::COMPRESSOR_OPTION_COMBINATIONS.each do |compressor_options|
                  compressed_buffer = ::StringIO.new
                  compressed_buffer.set_encoding ::Encoding::BINARY

                  writer = proc { |portion| compressed_buffer << portion }

                  compressor = Target.new compressor_options

                  begin
                    source      = "".b
                    text_offset = 0
                    index       = 0

                    loop do
                      portion = text.byteslice text_offset, portion_length
                      break if portion.nil?

                      text_offset += portion_length
                      source << portion

                      bytes_written = compressor.write source, &writer
                      source        = source.byteslice bytes_written, source.bytesize - bytes_written

                      compressor.flush(&writer) if index.even?
                      index += 1
                    end

                  ensure
                    refute compressor.closed?
                    compressor.close(&writer)
                    assert compressor.closed?
                  end

                  compressed_text = compressed_buffer.string

                  Option.get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                    decompressed_text = String.decompress compressed_text, decompressor_options
                    decompressed_text.force_encoding text.encoding

                    assert_equal text, decompressed_text
                  end
                end
              end
            end
          end
        end

        Minitest << Compressor
      end
    end
  end
end
