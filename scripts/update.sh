#!/bin/bash -l
set -e

cd "$(dirname $0)"

git fetch --all || true
git remote | xargs -n1 -I {} git rebase "{}/master" || true

cd ".."
rvm use "."
gem install bundler
bundle update
