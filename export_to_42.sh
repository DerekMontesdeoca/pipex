#!/usr/bin/env bash

set -euo pipefail

if [[ "#@" -ne 1 ]]; then
	echo "Must supply directory" 1>&2;
	exit 1
fi

source_dir="$(dirname "$0")"

rsync --files-from="$source_dir/to_export.txt" -av --mkpath --delete-after \
	"$source_dir" "$1"
