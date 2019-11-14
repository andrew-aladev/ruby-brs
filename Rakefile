require "rake/extensiontask"
require "rake/testtask"
require "rubygems/package_task"

load "ruby-brs.gemspec"

Rake::ExtensionTask.new do |ext|
  ext.name           = "brs_ext"
  ext.ext_dir        = "ext"
  ext.lib_dir        = "lib"
  ext.tmp_dir        = "tmp"
  ext.source_pattern = "*.{c,h}"
end

Rake::TestTask.new do |task|
  task.libs << %w[lib]

  pathes          = `find test | grep "\.test\.rb$"`
  task.test_files = ["test/coverage.rb"] + pathes.split("\n")
end

task :default => %i[compile test]

Gem::PackageTask.new(GEMSPEC).define
