# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs_ext"

require_relative "error"
require_relative "validation"

module BRS
  # BRS::Option module.
  module Option
    # Current default buffer length.
    DEFAULT_BUFFER_LENGTH = 0

    # Current compressor defaults.
    COMPRESSOR_DEFAULTS = {
      # Enables global VM lock where possible.
      :gvl                              => false,
      # Compressor mode.
      :mode                             => nil,
      # Compression level.
      :quality                          => nil,
      # Compressor window size.
      :lgwin                            => nil,
      # Compressor input block size.
      :lgblock                          => nil,
      # Recommended number of postfix bits.
      :npostfix                         => nil,
      # Recommended number of direct distance codes (step 1 << npostfix, max 15 << npostfix).
      :ndirect                          => nil,
      # Disables literal context modeling format.
      :disable_literal_context_modeling => nil,
      # Enables large window.
      :large_window                     => nil
    }
    .freeze

    # Current decompressor defaults.
    DECOMPRESSOR_DEFAULTS = {
      # Enables global VM lock where possible.
      :gvl                              => false,
      # Disables ring buffer reallocation.
      :disable_ring_buffer_reallocation => nil,
      # Enables large window.
      :large_window                     => nil
    }
    .freeze

    # Processes compressor +options+ and +buffer_length_names+.
    # Option: +:source_buffer_length+ source buffer length.
    # Option: +:destination_buffer_length+ destination buffer length.
    # Option: +:gvl+ enables global VM lock where possible.
    # Option: +:mode+ compressor mode.
    # Option: +:quality+ compression level.
    # Option: +:lgwin+ compressor window size.
    # Option: +:lgblock+ compressor input block size.
    # Option: +:npostfix+ recommended number of postfix bits.
    # Option: +:ndirect+ recommended number of direct distance codes (step 1 << npostfix, max 15 << npostfix).
    # Option: +:disable_literal_context_modeling+ Disables literal context modeling format.
    # Option: +:large_window+ enables large window.
    # Returns processed compressor options.
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

    # Processes decompressor +options+ and +buffer_length_names+.
    # Option: +:source_buffer_length+ source buffer length.
    # Option: +:destination_buffer_length+ destination buffer length.
    # Option: +:gvl+ enables global VM lock where possible.
    # Option: +:disable_ring_buffer_reallocation+ disables ring buffer reallocation.
    # Option: +:large_window+ enables large window.
    # Returns processed decompressor options.
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
