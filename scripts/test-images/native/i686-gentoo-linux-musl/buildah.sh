#!/bin/bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

source "../../utils.sh"
source "./env.sh"

pull "$FROM_IMAGE_NAME"
check_up_to_date

CONTAINER=$(from "$FROM_IMAGE_NAME")
config --arch="x86" --entrypoint "/home/entrypoint.sh"

run mkdir -p /home
copy ../../entrypoint.sh /home/

copy root/ /
build emerge -v \
  dev-vcs/git app-arch/brotli \
  dev-lang/ruby:2.7 virtual/rubygems

commit
