# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/option"
require "ocg"

require_relative "validation"

module BRS
  module Test
    module Option
      INVALID_MODES = (
        Validation::INVALID_SYMBOLS - [nil] + %i[invalid_mode]
      )
      .freeze

      INVALID_QUALITIES = (
        Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil] +
        [
          BRS::Option::MIN_QUALITY - 1,
          BRS::Option::MAX_QUALITY + 1
        ]
      )
      .freeze

      INVALID_LGWINS = (
        Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil] +
        [
          BRS::Option::MIN_LGWIN - 1,
          BRS::Option::MAX_LGWIN + 1
        ]
      )
      .freeze

      INVALID_LGBLOCKS = (
        Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil] +
        [
          BRS::Option::MIN_LGBLOCK - 1,
          BRS::Option::MAX_LGBLOCK + 1
        ]
      )
      .freeze

      private_class_method def self.get_common_invalid_options(buffer_length_names, &block)
        Validation::INVALID_HASHES.each do |invalid_hash|
          block.call invalid_hash
        end

        buffer_length_names.each do |name|
          (Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil]).each do |invalid_integer|
            yield({ name => invalid_integer })
          end
        end

        Validation::INVALID_BOOLS.each do |invalid_bool|
          yield({ :gvl => invalid_bool })
        end
      end

      def self.get_invalid_compressor_options(buffer_length_names, &block)
        get_common_invalid_options buffer_length_names, &block

        INVALID_MODES.each do |invalid_mode|
          yield({ :mode => invalid_mode })
        end

        INVALID_QUALITIES.each do |invalid_quality|
          yield({ :quality => invalid_quality })
        end

        INVALID_LGWINS.each do |invalid_lgwin|
          yield({ :lgwin => invalid_lgwin })
        end

        INVALID_LGBLOCKS.each do |invalid_lgblock|
          yield({ :lgblock => invalid_lgblock })
        end

        (Validation::INVALID_BOOLS - [nil]).each do |invalid_bool|
          yield({ :disable_literal_context_modeling => invalid_bool })
          yield({ :large_window => invalid_bool })
        end
      end

      def self.get_invalid_decompressor_options(buffer_length_names, &block)
        get_common_invalid_options buffer_length_names, &block

        (Validation::INVALID_BOOLS - [nil]).each do |invalid_bool|
          yield({ :disable_ring_buffer_reallocation => invalid_bool })
          yield({ :large_window => invalid_bool })
        end
      end

      # -----

      # "0" means default buffer length.
      BUFFER_LENGTHS = [
        0,
        1
      ]
      .freeze

      BOOLS = [
        true,
        false
      ]
      .freeze

      QUALITIES = [
        BRS::Option::MIN_QUALITY,
        BRS::Option::MAX_QUALITY
      ]
      .freeze

      LGWINS = [
        BRS::Option::MIN_LGWIN,
        BRS::Option::MAX_LGWIN
      ]
      .freeze

      LGBLOCKS = [
        BRS::Option::MIN_LGBLOCK,
        BRS::Option::MAX_LGBLOCK
      ]
      .freeze

      private_class_method def self.get_buffer_length_option_generator(buffer_length_names)
        OCG.new(
          Hash[buffer_length_names.map { |name| [name, BUFFER_LENGTHS] }]
        )
      end

      def self.get_compressor_options(buffer_length_names, &_block)
        buffer_length_generator = get_buffer_length_option_generator buffer_length_names

        # main

        general_generator = OCG.new(
          :mode    => BRS::Option::MODES,
          :quality => QUALITIES
        )

        window_generator = OCG.new(
          :lgwin        => LGWINS,
          :lgblock      => LGBLOCKS,
          :large_window => BOOLS
        )

        main_generator = general_generator.mix window_generator

        # thread

        thread_generator = OCG.new(
          :gvl => BOOLS
        )

        # other

        other_generator = OCG.new(
          :disable_literal_context_modeling => BOOLS
        )

        # complete

        complete_generator = buffer_length_generator.mix(main_generator).mix(thread_generator).mix other_generator

        yield complete_generator.next until complete_generator.finished?
      end

      private_class_method def self.get_decompressor_options(buffer_length_names, &_block)
        buffer_length_generator = get_buffer_length_option_generator buffer_length_names

        # main

        main_generator = OCG.new(
          :large_window => BOOLS
        )

        # thread

        thread_generator = OCG.new(
          :gvl => BOOLS
        )

        # other

        other_generator = OCG.new(
          :disable_ring_buffer_reallocation => BOOLS
        )

        # complete

        complete_generator = buffer_length_generator.mix(main_generator).mix(thread_generator).mix other_generator

        yield complete_generator.next until complete_generator.finished?
      end

      def self.get_compatible_decompressor_options(compressor_options, buffer_length_name_mapping, &_block)
        buffer_length_names = buffer_length_name_mapping.values

        get_decompressor_options(buffer_length_names) do |decompressor_options|
          same_buffer_length_values = buffer_length_name_mapping.all? do |compressor_name, decompressor_name|
            decompressor_options[decompressor_name] == compressor_options[compressor_name]
          end

          yield decompressor_options if
            same_buffer_length_values &&
            compressor_options[:large_window] == decompressor_options[:large_window]
        end
      end
    end
  end
end
