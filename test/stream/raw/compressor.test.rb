# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/stream/raw/compressor"

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

          def test_invalid_initialize
            Option::INVALID_COMPRESSOR_OPTIONS.each do |invalid_options|
              assert_raises ValidateError do
                Target.new invalid_options
              end
            end
          end
        end

        Minitest << Compressor
      end
    end
  end
end
