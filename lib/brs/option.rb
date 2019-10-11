# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require_relative "error"
require_relative "validation"

module BRS
  module Option
    DEFAULT_BUFFER_LENGTH = 0

    COMPRESSOR_DEFAULTS = {
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
      :disable_ring_buffer_reallocation => nil,
      :large_window                     => nil
    }
    .freeze

    def self.get_compressor_options(options, buffer_length_names)
      Validation.validate_hash options

      buffer_length_defaults = buffer_length_names.each_with_object({}) { |name, defaults| defaults[name] = DEFAULT_BUFFER_LENGTH }
      options                = COMPRESSOR_DEFAULTS.merge(buffer_length_defaults).merge options

      buffer_length_names.each { |name| Validation.validate_not_negative_integer options[name] }

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

    def self.get_decompressor_options(options, buffer_length_names)
      Validation.validate_hash options

      buffer_length_defaults = buffer_length_names.each_with_object({}) { |name, defaults| defaults[name] = DEFAULT_BUFFER_LENGTH }
      options                = DECOMPRESSOR_DEFAULTS.merge(buffer_length_defaults).merge options

      buffer_length_names.each { |name| Validation.validate_not_negative_integer options[name] }

      disable_ring_buffer_reallocation = options[:disable_ring_buffer_reallocation]
      Validation.validate_bool disable_ring_buffer_reallocation unless disable_ring_buffer_reallocation.nil?

      large_window = options[:large_window]
      Validation.validate_bool large_window unless large_window.nil?

      options
    end
  end
end
