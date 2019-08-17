# Ruby bindings for brotli library.
# Copyright (c) 2019 AUTHORS, MIT License.

require "date"

require_relative "lib/brs/version"

GEMSPEC = Gem::Specification.new do |gem|
  gem.name     = "ruby-brs"
  gem.summary  = "Ruby bindings for brs library."
  gem.homepage = "https://github.com/andrew-aladev/ruby-brs"
  gem.license  = "MIT"
  gem.authors  = File.read("AUTHORS").split("\n").reject(&:empty?)
  gem.email    = "aladjev.andrew@gmail.com"
  gem.version  = BRS::VERSION
  gem.date     = Date.today.to_s

  gem.add_development_dependency "minitar", "~> 0.8"
  gem.add_development_dependency "minitest", "~> 5.11"
  gem.add_development_dependency "rake-compiler", "~> 1.0"
  gem.add_development_dependency "rubocop", "~> 0.74"
  gem.add_development_dependency "rubocop-performance", "~> 1.4"
  gem.add_development_dependency "rubocop-rails", "~> 2.2"

  gem.files = \
    `git ls-files -z --directory {ext,lib}`.split("\x0") + \
    %w[AUTHORS LICENSE README.md]
  gem.require_paths = %w[lib]
  gem.extensions    = %w[ext/extconf.rb]
end
