# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs/option"

require_relative "validation"

module BRS
  module Test
    module Option
      INVALID_COMPRESSOR_OPTIONS = [
        Validation::INVALID_HASHES,
        (Validation::INVALID_SYMBOLS - [nil]).flat_map do |invalid_symbol|
          [
            { :mode => invalid_symbol },
            { :mode => :invalid_mode }
          ]
        end,
        (Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil]).flat_map do |invalid_integer|
          [
            { :buffer_length => invalid_integer },
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
      .flatten(1)
      .freeze

      INVALID_DECOMPRESSOR_OPTIONS = [
        Validation::INVALID_HASHES,
        (Validation::INVALID_NOT_NEGATIVE_INTEGERS - [nil]).map do |invalid_integer|
          { :buffer_length => invalid_integer }
        end,
        (Validation::INVALID_BOOLS - [nil]).flat_map do |invalid_bool|
          [
            { :disable_ring_buffer_reallocation => invalid_bool },
            { :large_window => invalid_bool }
          ]
        end
      ]
      .flatten(1)
      .freeze

      # -----

      # "0" means default buffer length.
      BUFFER_LENGTHS = [
        0,
        2,
        512
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

      COMPRESSOR_OPTION_DATA = [
        BUFFER_LENGTHS.map do |buffer_length|
          { :buffer_length => buffer_length }
        end,
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
      .freeze

      DECOMPRESSOR_OPTION_DATA = [
        BUFFER_LENGTHS.map do |buffer_length|
          { :buffer_length => buffer_length }
        end,
        BOOLS.map do |disable_ring_buffer_reallocation|
          { :disable_ring_buffer_reallocation => disable_ring_buffer_reallocation }
        end,
        BOOLS.map do |large_window|
          { :large_window => large_window }
        end
      ]
      .freeze

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

      COMPRESSOR_OPTION_COMBINATIONS   = get_option_combinations(COMPRESSOR_OPTION_DATA).freeze
      DECOMPRESSOR_OPTION_COMBINATIONS = get_option_combinations(DECOMPRESSOR_OPTION_DATA).freeze

      def self.get_compatible_decompressor_options(compressor_options, &_block)
        DECOMPRESSOR_OPTION_COMBINATIONS.each do |decompressor_options|
          yield decompressor_options if
            compressor_options[:buffer_length] == decompressor_options[:buffer_length] &&
            compressor_options[:large_window]  == decompressor_options[:large_window]
        end
      end
    end
  end
end
