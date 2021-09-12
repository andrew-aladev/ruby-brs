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

      INVALID_NPOSTFIXES = (
        Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil] +
        [
          BRS::Option::MIN_NPOSTFIX - 1,
          BRS::Option::MAX_NPOSTFIX + 1
        ]
      )
      .freeze

      INVALID_NDIRECTS = (
        Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil] +
        [
          BRS::Option::MIN_NDIRECT - 1,
          BRS::Option::MAX_NDIRECT + 1
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

        INVALID_NPOSTFIXES.each do |invalid_npostfix|
          yield({ :npostfix => invalid_npostfix })
        end

        INVALID_NDIRECTS.each do |invalid_ndirect|
          yield({ :ndirect => invalid_ndirect })
        end

        valid_npostfix = [BRS::Option::MIN_NPOSTFIX + 1, BRS::Option::MAX_NPOSTFIX].min

        # Ignoring ndirect step based on current npostfix.
        yield(
          :npostfix => valid_npostfix,
          :ndirect  => BRS::Option::MIN_NDIRECT + (BRS::Option::NDIRECT_NPOSTFIX_STEP_BASE << valid_npostfix) - 1
        )

        # Exceeding max ndirect based on current npostfix.
        yield(
          :npostfix => valid_npostfix,
          :ndirect  => BRS::Option::MIN_NDIRECT + (BRS::Option::MAX_NDIRECT_NPOSTFIX_BASE << valid_npostfix) + 1
        )

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

      private_class_method def self.get_option_values(values, min, max)
        values.map { |value| [[value, min].max, max].min }
      end

      # Absolute min and max values works too slow.
      # We can use more reasonable min and max values defined in "brotli/c/enc/encode.c".

      MODES = [
        BRS::Option::MODES.first
      ]
      .freeze

      QUALITIES = get_option_values(
        [1, 10],
        BRS::Option::MIN_QUALITY,
        BRS::Option::MAX_QUALITY
      )
      .freeze

      LGWINS = get_option_values(
        [10, 18],
        BRS::Option::MIN_LGWIN,
        BRS::Option::MAX_LGWIN
      )
      .freeze

      LGBLOCKS = get_option_values(
        [16, 22],
        BRS::Option::MIN_LGBLOCK,
        BRS::Option::MAX_LGBLOCK
      )
      .freeze

      NPOSTFIXES = get_option_values(
        [1, 2],
        BRS::Option::MIN_NPOSTFIX,
        BRS::Option::MAX_NPOSTFIX
      )
      .freeze

      def self.get_ndirects(npostfix)
        get_option_values(
          [
            BRS::Option::MIN_NDIRECT + (BRS::Option::NDIRECT_NPOSTFIX_STEP_BASE << npostfix),
            BRS::Option::MIN_NDIRECT + 2 * (BRS::Option::NDIRECT_NPOSTFIX_STEP_BASE << npostfix)
          ],
          BRS::Option::MIN_NDIRECT,
          BRS::Option::MIN_NDIRECT + (BRS::Option::MAX_NDIRECT_NPOSTFIX_BASE << npostfix)
        )
      end

      private_class_method def self.get_buffer_length_option_generator(buffer_length_names)
        OCG.new(
          buffer_length_names.map { |name| [name, BUFFER_LENGTHS] }.to_h
        )
      end

      def self.get_compressor_options_generator(buffer_length_names)
        buffer_length_generator = get_buffer_length_option_generator buffer_length_names

        # main

        general_generator = OCG.new(
          :mode    => MODES,
          :quality => QUALITIES
        )

        window_generator = OCG.new(
          :large_window => [true]
        )
        .or(
          :large_window => [false],
          :lgwin        => LGWINS
        )
        .and(
          :lgblock => LGBLOCKS
        )

        main_generator = general_generator.mix window_generator

        NPOSTFIXES.each do |npostfix|
          main_generator = main_generator.or(
            :npostfix => [npostfix],
            :ndirect  => get_ndirects(npostfix)
          )
        end

        # thread

        thread_generator = OCG.new(
          :gvl => BOOLS
        )

        # other

        other_generator = OCG.new(
          :disable_literal_context_modeling => BOOLS
        )

        # complete

        buffer_length_generator.mix(main_generator).mix(thread_generator).mix other_generator
      end

      def self.get_compatible_decompressor_options(compressor_options, buffer_length_name_mapping, &_block)
        decompressor_options = {
          :large_window => compressor_options[:large_window],
          :gvl          => compressor_options[:gvl]
        }

        buffer_length_name_mapping.each do |compressor_name, decompressor_name|
          decompressor_options[decompressor_name] = compressor_options[compressor_name]
        end

        other_generator = OCG.new(
          :disable_ring_buffer_reallocation => BOOLS
        )

        other_generator.each do |other_options|
          yield decompressor_options.merge(other_options)
        end
      end
    end
  end
end
