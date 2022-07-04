# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/validation"

module BRS
  # BRS::Validation class.
  class Validation < ADSP::Validation
    # Raises error when +value+ is not boolean.
    def self.validate_bool(value)
      raise ValidateError, "invalid bool" unless value.is_a?(::TrueClass) || value.is_a?(::FalseClass)
    end
  end
end
