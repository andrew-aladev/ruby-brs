#!/bin/bash
set -e

cd "$(dirname $0)"

env-update
source /etc/profile

git clone "https://github.com/andrew-aladev/ruby-brs.git" --single-branch --branch "master" --depth 1 "ruby-brs"
cd "ruby-brs"

./scripts/ci_test.sh
