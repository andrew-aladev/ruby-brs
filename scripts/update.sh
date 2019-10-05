#!/bin/bash -l
set -e

git fetch --all || true
git remote | xargs -n1 -I {} git rebase "{}/master" || true

gem install bundler
bundle update
