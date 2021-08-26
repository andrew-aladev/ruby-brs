# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "brs"

require_relative "minitest"

module BRS
  module Test
    class Version < Minitest::Test
      def test_versions
        refute_nil BRS::VERSION
        refute_nil BRS::LIBRARY_VERSION
      end
    end

    Minitest << Version
  end
end
