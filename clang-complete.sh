#!/bin/sh
set -e

cd "$(dirname $0)"

ruby_version=$(<.ruby-version)

echo "\
-I$(pwd)/ext
-I$HOME/.rvm/rubies/$ruby_version/include/"${ruby_version%.*}.0"" > ".clang_complete\
"
