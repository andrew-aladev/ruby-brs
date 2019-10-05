# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require_relative "error"
require_relative "validation"

module BRS
  module Option
    COMPRESSOR_DEFAULTS = {
      :buffer_length                    => 0,
      :mode                             => nil,
      :quality                          => nil,
      :lgwin                            => nil,
      :lgblock                          => nil,
      :disable_literal_context_modeling => nil,
      :size_hint                        => nil,
      :large_window                     => nil
    }
    .freeze

    DECOMPRESSOR_DEFAULTS = {
      :buffer_length                    => 0,
      :disable_ring_buffer_reallocation => nil,
      :large_window                     => nil
    }
    .freeze

    def self.get_compressor_options(options)
      Validation.validate_hash options

      options = COMPRESSOR_DEFAULTS.merge options

      Validation.validate_not_negative_integer options[:buffer_length]

      mode = options[:mode]
      unless mode.nil?
        Validation.validate_symbol mode
        raise ValidateError, "invalid mode" unless MODES.include? mode
      end

      quality = options[:quality]
      Validation.validate_not_negative_integer quality unless quality.nil?

      lgwin = options[:lgwin]
      Validation.validate_not_negative_integer lgwin unless lgwin.nil?

      lgblock = options[:lgblock]
      Validation.validate_not_negative_integer lgblock unless lgblock.nil?

      disable_literal_context_modeling = options[:disable_literal_context_modeling]
      Validation.validate_bool disable_literal_context_modeling unless disable_literal_context_modeling.nil?

      size_hint = options[:size_hint]
      Validation.validate_not_negative_integer size_hint unless size_hint.nil?

      large_window = options[:large_window]
      Validation.validate_bool large_window unless large_window.nil?

      options
    end

    def self.get_decompressor_options(options)
      Validation.validate_hash options

      options = DECOMPRESSOR_DEFAULTS.merge options

      Validation.validate_not_negative_integer options[:buffer_length]

      disable_ring_buffer_reallocation = options[:disable_ring_buffer_reallocation]
      Validation.validate_bool disable_ring_buffer_reallocation unless disable_ring_buffer_reallocation.nil?

      large_window = options[:large_window]
      Validation.validate_bool large_window unless large_window.nil?

      options
    end
  end
end
