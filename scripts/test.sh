#!/bin/bash

set -uo pipefail

length_csv=`wc -l benchmark.csv | cut -d ' ' -f1`
num_instances=`head -n 1 benchmark.csv | cut -d ' ' -f4`

if [[ `expr $num_instances \* 6 + 2` -ne $length_csv ]]; then
  echo Benchmark incomplete or contains invalid instances! >&2
  exit 1
fi

tail -n +3 benchmark.csv | cut -d ',' -f2 | grep -q 0
if [[ $? -eq 0 ]]; then
  echo Some instances where solved incorrectly! >&2
  exit 1
fi
