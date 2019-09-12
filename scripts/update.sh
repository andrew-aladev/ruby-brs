#!/bin/sh
set -e

cd "$(dirname $0)"

git fetch --all || true
git remote | xargs -n1 -I {} git rebase "{}/master" || true

cd ".."
gem install bundler
bundle update
