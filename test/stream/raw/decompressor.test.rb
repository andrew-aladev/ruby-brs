# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/stream/raw/decompressor"

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

          TEXTS           = Common::TEXTS
          PORTION_LENGTHS = Common::PORTION_LENGTHS

          DECOMPRESSOR_OPTION_COMBINATIONS = Option::DECOMPRESSOR_OPTION_COMBINATIONS

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

            decompressor.close(&NOOP_PROC)

            assert_raises UsedAfterCloseError do
              decompressor.read "", &NOOP_PROC
            end
          end
        end

        Minitest << Decompressor
      end
    end
  end
end
