# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "parallel"
require "securerandom"

module BRS
  module Test
    module Common
      BASE_PATH = ::File.expand_path(::File.join(::File.dirname(__FILE__), "..")).freeze
      TEMP_PATH = ::File.join(BASE_PATH, "tmp").freeze

      SOURCE_PATH  = ::File.join(TEMP_PATH, "source").freeze
      ARCHIVE_PATH = ::File.join(TEMP_PATH, "archive").freeze

      [
        SOURCE_PATH,
        ARCHIVE_PATH
      ]
      .each { |path| FileUtils.touch path }

      ENCODINGS = %w[
        binary
        UTF-8
        UTF-16LE
      ]
      .map { |encoding_name| ::Encoding.find encoding_name }
      .freeze

      TRANSCODE_OPTIONS = {
        :invalid => :replace,
        :undef   => :replace,
        :replace => "?"
      }
      .freeze

      def self.generate_texts(*sources)
        sources.flat_map do |source|
          ENCODINGS.map do |encoding|
            source.encode encoding, **TRANSCODE_OPTIONS
          end
        end
      end

      TEXTS = generate_texts(
        "",
        ::SecureRandom.random_bytes(1 << 8) # 256 B
      )
      .freeze

      LARGE_TEXTS = generate_texts(
        ::SecureRandom.random_bytes(1 << 21) # 2 MB
      )
      .freeze

      # It is better to have text lengths not divisible by portion lengths.
      PORTION_LENGTHS = [
        10**2,
        5 * (10**2)
      ]
      .freeze

      # It is better to have large text lengths not divisible by large portion lengths.
      LARGE_PORTION_LENGTHS = [
        10**6,
        5 * (10**6)
      ]
      .freeze

      # We need at least 2 threads for testing multiple threads support.
      THREADS_COUNT = [Parallel.processor_count, 2].max.freeze

      def self.parallel_each(producer, &_block)
        Parallel.each producer, :in_threads => THREADS_COUNT do |item|
          yield item, Parallel.worker_number
        end
      end
    end
  end
end
