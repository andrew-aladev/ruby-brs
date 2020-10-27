# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/stream/reader"
require "brs/string"
require "English"
require "stringio"

require_relative "../common"
require_relative "../minitest"
require_relative "../option"
require_relative "../validation"

module BRS
  module Test
    module Stream
      class ReaderHelpers < Minitest::Test
        Target = BRS::Stream::Reader
        String = BRS::String

        ARCHIVE_PATH      = Common::ARCHIVE_PATH
        ENCODINGS         = Common::ENCODINGS
        TRANSCODE_OPTIONS = Common::TRANSCODE_OPTIONS
        TEXTS             = Common::TEXTS
        LARGE_TEXTS       = Common::LARGE_TEXTS

        BUFFER_LENGTH_NAMES   = %i[source_buffer_length destination_buffer_length].freeze
        BUFFER_LENGTH_MAPPING = {
          :source_buffer_length      => :destination_buffer_length,
          :destination_buffer_length => :source_buffer_length
        }
        .freeze

        LIMITS = [nil, 1].freeze

        def test_invalid_ungetbyte
          instance = target.new ::StringIO.new

          Validation::INVALID_STRINGS.each do |invalid_string|
            assert_raises ValidateError do
              instance.ungetbyte invalid_string
            end
          end
        end

        def test_byte
          Common.parallel TEXTS do |text, worker_index|
            archive_path = "#{ARCHIVE_PATH}_#{worker_index}"

            get_compressor_options do |compressor_options|
              write_archive archive_path, text, compressor_options

              get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                Target.open archive_path, decompressor_options do |instance|
                  # getbyte

                  byte = instance.getbyte
                  instance.ungetbyte byte unless byte.nil?

                  # readbyte

                  begin
                    byte = instance.readbyte
                    instance.ungetc byte
                  rescue ::EOFError
                    # ok
                  end

                  # each_byte

                  decompressed_text = "".b
                  instance.each_byte { |current_byte| decompressed_text << current_byte }

                  decompressed_text.force_encoding text.encoding
                  assert_equal text, decompressed_text
                end
              end
            end
          end
        end

        # -- char --

        def test_invalid_ungetc
          instance = target.new ::StringIO.new

          Validation::INVALID_STRINGS.each do |invalid_string|
            assert_raises ValidateError do
              instance.ungetc invalid_string
            end
          end
        end

        def test_char
          Common.parallel TEXTS do |text, worker_index|
            archive_path = "#{ARCHIVE_PATH}_#{worker_index}"

            get_compressor_options do |compressor_options|
              write_archive archive_path, text, compressor_options

              get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                Target.open archive_path, decompressor_options do |instance|
                  # getc

                  char = instance.getc
                  instance.ungetc char unless char.nil?

                  # readchar

                  begin
                    char = instance.readchar
                    instance.ungetc char
                  rescue ::EOFError
                    # ok
                  end

                  # each_char

                  decompressed_text = "".b
                  instance.each_char { |current_char| decompressed_text << current_char }

                  decompressed_text.force_encoding text.encoding
                  assert_equal text, decompressed_text
                end
              end
            end
          end
        end

        def test_char_encoding
          Common.parallel TEXTS do |text, worker_index|
            archive_path      = "#{ARCHIVE_PATH}_#{worker_index}"
            external_encoding = text.encoding

            (ENCODINGS - [external_encoding]).each do |internal_encoding|
              target_text = text.encode internal_encoding, **TRANSCODE_OPTIONS

              get_compressor_options do |compressor_options|
                write_archive archive_path, text, compressor_options

                get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                  Target.open archive_path, decompressor_options do |instance|
                    instance.set_encoding external_encoding, internal_encoding, **TRANSCODE_OPTIONS

                    # getc

                    char = instance.getc

                    unless char.nil?
                      assert_equal char.encoding, internal_encoding
                      instance.ungetc char
                    end

                    # readchar

                    begin
                      char = instance.readchar
                      assert_equal char.encoding, internal_encoding

                      instance.ungetc char
                    rescue ::EOFError
                      # ok
                    end

                    # each_char

                    decompressed_text = ::String.new :encoding => internal_encoding

                    instance.each_char do |current_char|
                      assert_equal current_char.encoding, internal_encoding
                      decompressed_text << current_char
                    end

                    assert_equal target_text, decompressed_text
                  end
                end
              end
            end
          end
        end

        # -- lines --

        def test_invalid_gets
          instance = target.new ::StringIO.new

          (Validation::INVALID_STRINGS - [nil, 1, 1.1]).each do |invalid_string|
            assert_raises ValidateError do
              instance.gets invalid_string
            end
          end

          (Validation::INVALID_POSITIVE_INTEGERS - [nil]).each do |invalid_integer|
            assert_raises ValidateError do
              instance.gets nil, invalid_integer
            end
          end
        end

        def test_invalid_ungetline
          instance = target.new ::StringIO.new

          Validation::INVALID_STRINGS.each do |invalid_string|
            assert_raises ValidateError do
              instance.ungetline invalid_string
            end
          end
        end

        def test_lines
          Common.parallel TEXTS do |text, worker_index|
            archive_path = "#{ARCHIVE_PATH}_#{worker_index}"

            separator =
              if text.empty?
                nil
              else
                text[0]
              end

            get_compressor_options do |compressor_options|
              write_archive archive_path, text, compressor_options

              get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                Target.open archive_path, decompressor_options do |instance|
                  # lineno

                  assert_equal instance.lineno, 0

                  instance.lineno = 1
                  assert_equal instance.lineno, 1

                  instance.lineno = 0
                  assert_equal instance.lineno, 0

                  # gets

                  LIMITS.each do |limit|
                    line = instance.gets limit
                    next if line.nil?

                    assert_equal instance.lineno, 1

                    instance.ungetline line
                    assert_equal instance.lineno, 0

                    # Same test with separator.

                    line = instance.gets separator, limit
                    next if line.nil?

                    assert_equal instance.lineno, 1

                    instance.ungetline line
                    assert_equal instance.lineno, 0
                  end

                  # readline

                  begin
                    line = instance.readline
                    assert_equal instance.lineno, 1

                    instance.ungetline line
                    assert_equal instance.lineno, 0
                  rescue ::EOFError
                    # ok
                  end

                  # readlines

                  lines = instance.readlines
                  lines.each { |current_line| instance.ungetline current_line }

                  decompressed_text = lines.join ""
                  decompressed_text.force_encoding text.encoding

                  assert_equal text, decompressed_text

                  # each_line

                  decompressed_text = "".b
                  instance.each_line { |current_line| decompressed_text << current_line }

                  decompressed_text.force_encoding text.encoding
                  assert_equal text, decompressed_text
                end
              end
            end
          end
        end

        def test_lines_encoding
          Common.parallel TEXTS do |text, worker_index|
            archive_path      = "#{ARCHIVE_PATH}_#{worker_index}"
            external_encoding = text.encoding

            (ENCODINGS - [external_encoding]).each do |internal_encoding|
              target_text = text.encode internal_encoding, **TRANSCODE_OPTIONS

              separator =
                if text.empty?
                  nil
                else
                  text[0]
                end

              get_compressor_options do |compressor_options|
                write_archive archive_path, text, compressor_options

                get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                  Target.open archive_path, decompressor_options do |instance|
                    instance.set_encoding external_encoding, internal_encoding, TRANSCODE_OPTIONS

                    # gets

                    line = instance.gets separator

                    unless line.nil?
                      assert_equal line.encoding, internal_encoding
                      instance.ungetline line
                    end

                    # readline

                    begin
                      line = instance.readline
                      assert_equal line.encoding, internal_encoding

                      instance.ungetline line
                    rescue ::EOFError
                      # ok
                    end

                    # each_line

                    decompressed_text = ::String.new :encoding => internal_encoding

                    instance.each_line do |current_line|
                      assert_equal current_line.encoding, internal_encoding
                      decompressed_text << current_line
                    end

                    assert_equal target_text, decompressed_text
                  end
                end
              end
            end
          end
        end

        # -- etc --

        def test_invalid_open
          Validation::INVALID_STRINGS.each do |invalid_string|
            assert_raises ValidateError do
              Target.open(invalid_string) {}
            end
          end

          # Proc is required.
          assert_raises ValidateError do
            Target.open ARCHIVE_PATH
          end
        end

        def test_open
          Common.parallel TEXTS do |text, worker_index|
            archive_path = "#{ARCHIVE_PATH}_#{worker_index}"

            get_compressor_options do |compressor_options|
              write_archive archive_path, text, compressor_options

              get_compatible_decompressor_options(compressor_options) do |decompressor_options|
                decompressed_text = Target.open archive_path, decompressor_options, &:read
                decompressed_text.force_encoding text.encoding

                assert_equal text, decompressed_text
              end
            end
          end
        end

        def test_open_with_large_texts
          Common.parallel LARGE_TEXTS do |text, worker_index|
            archive_path = "#{ARCHIVE_PATH}_#{worker_index}"
            write_archive archive_path, text

            decompressed_text = Target.open archive_path, &:read
            decompressed_text.force_encoding text.encoding

            assert_equal text, decompressed_text
          end
        end

        # -----

        protected def write_archive(archive_path, text, compressor_options = {})
          compressed_text = String.compress text, compressor_options
          ::File.write archive_path, compressed_text
        end

        def get_compressor_options(&block)
          Option.get_compressor_options BUFFER_LENGTH_NAMES, &block
        end

        def get_compatible_decompressor_options(compressor_options, &block)
          Option.get_compatible_decompressor_options compressor_options, BUFFER_LENGTH_MAPPING, &block
        end

        protected def target
          self.class::Target
        end
      end

      Minitest << ReaderHelpers
    end
  end
end
