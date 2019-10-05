# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs_ext"

require_relative "abstract"
require_relative "../../option"
require_relative "../../validation"

module BRS
  module Stream
    module Raw
      class Decompressor < Abstract
        def initialize(options = {})
          options       = Option.get_decompressor_options options
          native_stream = NativeDecompressor.new options

          super native_stream
        end

        def read(source, &writer)
          do_not_use_after_close

          Validation.validate_string source
          Validation.validate_proc writer

          total_bytes_read = 0

          loop do
            bytes_read, need_more_destination  = @native_stream.read source
            total_bytes_read                  += bytes_read

            if need_more_destination
              source = source.byteslice bytes_read, source.bytesize - bytes_read
              flush_destination_buffer(&writer)
              next
            end

            break
          end

          # Please remember that "total_bytes_read" can not be equal to "source.bytesize".
          total_bytes_read
        end

        def flush(&writer)
          do_not_use_after_close

          Validation.validate_proc writer

          super

          nil
        end

        def close(&writer)
          return nil if closed?

          Validation.validate_proc writer

          super

          nil
        end
      end
    end
  end
end
