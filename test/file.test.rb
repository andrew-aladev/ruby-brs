# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/file"
require "brs/file"

require_relative "minitest"
require_relative "option"

module BRS
  module Test
    class File < ADSP::Test::File
      Target = BRS::File
      Option = BRS::Test::Option
    end

    Minitest << File
  end
end
