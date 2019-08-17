# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "mkmf"

def require_header(name)
  abort "Can't find #{name} header" unless find_header name
end

#require_header ""

def require_library(name, functions)
  functions.each do |function|
    abort "Can't find #{name} library and #{function} function" unless find_library name, function
  end
end

functions = %w[
]
.freeze

require_library "brotli", functions

extension_name = "brs_ext".freeze
dir_config extension_name

sources = %w[
]
.freeze

# rubocop:disable Style/GlobalVars
$srcs = sources
  .map { |name| "src/#{extension_name}/#{name}.c" }
  .freeze

$CFLAGS << " -Wno-declaration-after-statement"
$VPATH << "$(srcdir)/#{extension_name}:$(srcdir)/#{extension_name}/stream"
# rubocop:enable Style/GlobalVars

create_makefile extension_name
