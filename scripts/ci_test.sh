#!/bin/bash
set -e

cd "$(dirname $0)"

# This script is for CI machines only, it provides junk and changes some config files.
# Please do not use it on your machine.

cd ".."

# CI may not want to provide target ruby version.
# We can just use the latest available ruby based on target major version.
if command -v rvm > /dev/null 2>&1; then
  ruby_version=$(< ".ruby-version")
  ruby_major_version=$(echo "${ruby_version%.*}" | sed "s/\./\\\./g") # escaping for regex
  ruby_version=$(rvm list | grep -o -e "$ruby_major_version\.[0-9]\+" | sort | tail -n 1)
  echo "$ruby_version" > ".ruby-version"
fi

bash -cl "\
  rvm use '.'; \
  gem install bundler && \
  bundle install && \
  bundle exec rake clean && \
  bundle exec rake \
"
