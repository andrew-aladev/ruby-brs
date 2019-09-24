# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

module BRS
  class BaseError < ::StandardError; end

  class AllocateError < BaseError; end
  class ValidateError < BaseError; end

  class UsedAfterCloseError              < BaseError; end
  class NotEnoughDestinationError        < BaseError; end
  class NotEnoughDestinationBufferError  < BaseError; end
  class DecompressorCorruptedSourceError < BaseError; end

  class AccessIOError < BaseError; end
  class ReadIOError   < BaseError; end
  class WriteIOError  < BaseError; end

  class UnexpectedError < BaseError; end
end
