#!/usr/bin/env bash

set -euo pipefail

if [[ "$#" -ne 1 ]]; then
	echo "Must supply directory" 1>&2;
	exit 1
fi

source_dir="$(dirname "$0")"

make -C "$source_dir" fclean

rsync --files-from="$source_dir/to_export.txt" -avr --mkpath \
	"$source_dir" "$1"
