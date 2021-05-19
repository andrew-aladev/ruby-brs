#!/usr/bin/env bash
set -e

DIR=$(dirname "${BASH_SOURCE[0]}")
cd "$DIR"

source "../../utils.sh"
source "./env.sh"

fusermount -zu attached-common-root || :
bindfs -r -o nonempty "../../data/common-root" attached-common-root
build "FROM_IMAGE" || error=$?
fusermount -zu attached-common-root || :

if [ ! -z "$error" ]; then
  exit "$error"
fi
