#!/usr/bin/env bash

export DEPTH=0

if [[ "$OSTYPE" == "darwin"* ]]; then
  DATE_CMD="gdate"
else
  DATE_CMD="date"
fi

time_now_ms () {
  "$DATE_CMD" "+%s%3N"
}

indented () {
  for ((i=0; i < DEPTH; ++i)); do
    echo -n "  " 1>&2
  done

  echo "$@" 1>&2
}

run () {
  { set +e; } 2>/dev/null

  if [ "$verbose" = "true" ]; then
    indented "+ $@"
  fi

  start=$(time_now_ms)

  "$@"
  result="$?"

  end=$(time_now_ms)

  { set -e; } 2>/dev/null

  if [ "$verbose" = "true" ]; then
    time=$((end - start))
    time_s=$(bc <<< "scale=3; $time/1000" | sed -e 's/^\./0./')

    indented "  time=${time_s}"
  fi

  if [ "$result" -ne 0 ]; then
    exit "$result"
  fi
}
