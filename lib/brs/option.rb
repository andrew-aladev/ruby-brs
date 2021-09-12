# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs_ext"

require_relative "error"
require_relative "validation"

module BRS
  module Option
    DEFAULT_BUFFER_LENGTH = 0

    COMPRESSOR_DEFAULTS = {
      :gvl                              => false,
      :mode                             => nil,
      :quality                          => nil,
      :lgwin                            => nil,
      :lgblock                          => nil,
      :npostfix                         => nil,
      :ndirect                          => nil,
      :disable_literal_context_modeling => nil,
      :large_window                     => nil
    }
    .freeze

    DECOMPRESSOR_DEFAULTS = {
      :gvl                              => false,
      :disable_ring_buffer_reallocation => nil,
      :large_window                     => nil
    }
    .freeze

    def self.get_compressor_options(options, buffer_length_names)
      Validation.validate_hash options

      buffer_length_defaults = buffer_length_names.each_with_object({}) do |name, defaults|
        defaults[name] = DEFAULT_BUFFER_LENGTH
      end

      options = COMPRESSOR_DEFAULTS.merge(buffer_length_defaults).merge options

      buffer_length_names.each { |name| Validation.validate_not_negative_integer options[name] }

      Validation.validate_bool options[:gvl]

      mode = options[:mode]
      unless mode.nil?
        Validation.validate_symbol mode
        raise ValidateError, "invalid mode" unless MODES.include? mode
      end

      quality = options[:quality]
      unless quality.nil?
        Validation.validate_not_negative_integer quality
        raise ValidateError, "invalid quality" if quality < MIN_QUALITY || quality > MAX_QUALITY
      end

      lgwin = options[:lgwin]
      unless lgwin.nil?
        Validation.validate_not_negative_integer lgwin
        raise ValidateError, "invalid lgwin" if lgwin < MIN_LGWIN || lgwin > MAX_LGWIN
      end

      lgblock = options[:lgblock]
      unless lgblock.nil?
        Validation.validate_not_negative_integer lgblock
        raise ValidateError, "invalid lgblock" if lgblock < MIN_LGBLOCK || lgblock > MAX_LGBLOCK
      end

      npostfix = options[:npostfix]
      unless npostfix.nil?
        Validation.validate_not_negative_integer npostfix
        raise ValidateError, "invalid npostfix" if npostfix < MIN_NPOSTFIX || npostfix > MAX_NPOSTFIX
      end

      ndirect = options[:ndirect]
      unless ndirect.nil?
        Validation.validate_not_negative_integer ndirect
        raise ValidateError, "invalid ndirect" if ndirect < MIN_NDIRECT || ndirect > MAX_NDIRECT

        raise ValidateError, "invalid ndirect" if
          !npostfix.nil? && (
            (ndirect - MIN_NDIRECT) % (NDIRECT_NPOSTFIX_STEP_BASE << npostfix) != 0 ||
            (ndirect - MIN_NDIRECT) > (NDIRECT_NPOSTFIX_MAX_BASE << npostfix)
          )
      end

      disable_literal_context_modeling = options[:disable_literal_context_modeling]
      Validation.validate_bool disable_literal_context_modeling unless disable_literal_context_modeling.nil?

      large_window = options[:large_window]
      Validation.validate_bool large_window unless large_window.nil?

      options
    end

    def self.get_decompressor_options(options, buffer_length_names)
      Validation.validate_hash options

      buffer_length_defaults = buffer_length_names.each_with_object({}) do |name, defaults|
        defaults[name] = DEFAULT_BUFFER_LENGTH
      end

      options = DECOMPRESSOR_DEFAULTS.merge(buffer_length_defaults).merge options

      buffer_length_names.each { |name| Validation.validate_not_negative_integer options[name] }

      Validation.validate_bool options[:gvl]

      disable_ring_buffer_reallocation = options[:disable_ring_buffer_reallocation]
      Validation.validate_bool disable_ring_buffer_reallocation unless disable_ring_buffer_reallocation.nil?

      large_window = options[:large_window]
      Validation.validate_bool large_window unless large_window.nil?

      options
    end
  end
end
