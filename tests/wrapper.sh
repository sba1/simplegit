#!/bin/sh

export PATH="${BIN_GIT_DIRECTORY}:${PATH}";

exec "${BIN_GIT2}" "$@";
