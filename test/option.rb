# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/option"
require "ocg"

require_relative "validation"

module BRS
  module Test
    module Option
      private_class_method def self.get_invalid_buffer_length_options(buffer_length_names, &_block)
        buffer_length_names.each do |name|
          (Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil]).each do |invalid_integer|
            yield({ name => invalid_integer })
          end
        end
      end

      def self.get_invalid_compressor_options(buffer_length_names, &block)
        Validation::INVALID_HASHES.each do |invalid_hash|
          yield invalid_hash
        end

        get_invalid_buffer_length_options buffer_length_names, &block

        yield({ :mode => :invalid_mode })

        yield({ :quality => BRS::Option::MIN_QUALITY - 1 })
        yield({ :quality => BRS::Option::MAX_QUALITY + 1 })

        yield({ :lgwin => BRS::Option::MIN_LGWIN - 1 })
        yield({ :lgwin => BRS::Option::MAX_LGWIN + 1 })

        yield({ :lgblock => BRS::Option::MIN_LGBLOCK - 1 })
        yield({ :lgblock => BRS::Option::MAX_LGBLOCK + 1 })

        (Validation::INVALID_SYMBOLS - [nil]).each do |invalid_symbol|
          yield({ :mode => invalid_symbol })
        end

        (Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil]).each do |invalid_integer|
          yield({ :quality => invalid_integer })
          yield({ :lgwin => invalid_integer })
          yield({ :lgblock => invalid_integer })
          yield({ :size_hint => invalid_integer })
        end

        (Validation::INVALID_BOOLS - [nil]).each do |invalid_bool|
          yield({ :disable_literal_context_modeling => invalid_bool })
          yield({ :large_window => invalid_bool })
        end
      end

      def self.get_invalid_decompressor_options(buffer_length_names, &block)
        Validation::INVALID_HASHES.each do |invalid_hash|
          yield invalid_hash
        end

        get_invalid_buffer_length_options buffer_length_names, &block

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

      # We can ignore "size_hint" option.

      private_class_method def self.get_buffer_length_option_generator(buffer_length_names)
        OCG.new(
          Hash[buffer_length_names.map { |name| [name, BUFFER_LENGTHS] }]
        )
      end

      def self.get_compressor_options(buffer_length_names, &_block)
        buffer_length_generator = get_buffer_length_option_generator buffer_length_names

        main_generator = OCG.new(
          :mode                             => BRS::Option::MODES,
          :quality                          => QUALITIES,
          :lgwin                            => LGWINS,
          :lgblock                          => LGBLOCKS,
          :disable_literal_context_modeling => BOOLS,
          :large_window                     => BOOLS
        )

        complete_generator = buffer_length_generator.and main_generator

        yield complete_generator.next until complete_generator.finished?
      end

      private_class_method def self.get_decompressor_options(buffer_length_names, &_block)
        buffer_length_generator = get_buffer_length_option_generator buffer_length_names

        main_generator = OCG.new(
          :disable_ring_buffer_reallocation => BOOLS,
          :large_window                     => BOOLS
        )

        complete_generator = buffer_length_generator.and main_generator

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
