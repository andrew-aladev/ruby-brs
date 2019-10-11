# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/option"

require_relative "validation"

module BRS
  module Test
    module Option
      private_class_method def self.get_invalid_buffer_length_options(buffer_length_names)
        buffer_length_names.flat_map do |name|
          (Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil]).map do |invalid_integer|
            { name => invalid_integer }
          end
        end
      end

      def self.get_invalid_compressor_options(buffer_length_names)
        [
          Validation::INVALID_HASHES,
          get_invalid_buffer_length_options(buffer_length_names),
          (Validation::INVALID_SYMBOLS - [nil]).flat_map do |invalid_symbol|
            [
              { :mode => invalid_symbol },
              { :mode => :invalid_mode }
            ]
          end,
          (Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil]).flat_map do |invalid_integer|
            [
              { :quality => invalid_integer },
              { :lgwin => invalid_integer },
              { :lgblock => invalid_integer },
              { :size_hint => invalid_integer }
            ]
          end,
          (Validation::INVALID_BOOLS - [nil]).flat_map do |invalid_bool|
            [
              { :disable_literal_context_modeling => invalid_bool },
              { :large_window => invalid_bool }
            ]
          end
        ]
        .flatten 1
      end

      def self.get_invalid_decompressor_options(buffer_length_names)
        [
          Validation::INVALID_HASHES,
          get_invalid_buffer_length_options(buffer_length_names),
          (Validation::INVALID_BOOLS - [nil]).flat_map do |invalid_bool|
            [
              { :disable_ring_buffer_reallocation => invalid_bool },
              { :large_window => invalid_bool }
            ]
          end
        ]
        .flatten 1
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

      private_class_method def self.get_compressor_option_data(buffer_length_names)
        [
          buffer_length_names.map do |name|
            BUFFER_LENGTHS.map do |buffer_length|
              { name => buffer_length }
            end
          end,
          [
            BRS::Option::MODES.map do |mode|
              { :mode => mode }
            end,
            QUALITIES.map do |quality|
              { :quality => quality }
            end,
            LGWINS.map do |lgwin|
              { :lgwin => lgwin }
            end,
            LGBLOCKS.map do |lgblock|
              { :lgblock => lgblock }
            end,
            BOOLS.map do |disable_literal_context_modeling|
              { :disable_literal_context_modeling => disable_literal_context_modeling }
            end,
            BOOLS.map do |large_window|
              { :large_window => large_window }
            end
          ]
        ]
        .flatten 1
      end

      private_class_method def self.get_decompressor_option_data(buffer_length_names)
        [
          buffer_length_names.map do |name|
            BUFFER_LENGTHS.map do |buffer_length|
              { name => buffer_length }
            end
          end,
          [
            BOOLS.map do |disable_ring_buffer_reallocation|
              { :disable_ring_buffer_reallocation => disable_ring_buffer_reallocation }
            end,
            BOOLS.map do |large_window|
              { :large_window => large_window }
            end
          ]
        ]
        .flatten 1
      end

      private_class_method def self.get_option_combinations(data)
        combinations = data
          .inject([]) do |result, array|
            next array if result.empty?

            result
              .product(array)
              .map(&:flatten)
          end

        combinations.map do |options|
          options.reduce({}, :merge)
        end
      end

      def self.get_compressor_option_combinations(buffer_length_names, &_block)
        get_option_combinations get_compressor_option_data(buffer_length_names)
      end

      def self.get_compatible_decompressor_options(compressor_options, buffer_length_name_mapping, &_block)
        buffer_length_names              = buffer_length_name_mapping.values
        decompressor_option_combinations = get_option_combinations get_decompressor_option_data(buffer_length_names)

        decompressor_option_combinations.each do |decompressor_options|
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
