#!/bin/bash

set -e
set -u

DIR="$(readlink -f "$(dirname "$0")")"
pushd "$DIR"

cppcheck \
    --enable=all \
    --inconclusive \
    --std=c99 \
    --force \
    --suppress=missingIncludeSystem \
    --error-exitcode=1 \
    --quiet \
    -Iinclude \
    -Isrc \
    $(pkg-config --cflags-only-I glib-2.0) \
    $(pkg-config --cflags-only-I gtk+-2.0) \
    $(pkg-config --cflags-only-I pidgin) \
    .
