# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

module BRS
  class BaseError < ::StandardError; end

  class ValidateError   < BaseError; end
  class AllocateError   < BaseError; end
  class UnexpectedError < BaseError; end

  class NotEnoughDestinationError < BaseError; end
  class UsedAfterCloseError       < BaseError; end
end
