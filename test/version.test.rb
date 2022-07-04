# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "adsp/test/version"
require "brs"

require_relative "minitest"

module BRS
  module Test
    class Version < ADSP::Test::Version
      def test_version
        refute_nil BRS::VERSION
        refute_nil BRS::LIBRARY_VERSION
      end
    end

    Minitest << Version
  end
end
