#!/bin/sh
set -e

if [ $(echo "$1" | cut -c1) = "-" ]; then
  echo "$0: assuming arguments for pexad"

  set -- pexad "$@"
fi

if [ $(echo "$1" | cut -c1) = "-" ] || [ "$1" = "pexad" ]; then
  mkdir -p "$PEXA_DATA"
  chmod 700 "$PEXA_DATA"
  chown -R root "$PEXA_DATA"

  echo "$0: setting data directory to $PEXA_DATA"

  set -- "$@" -datadir="$PEXA_DATA"
fi

if [ "$1" = "pexad" ] || [ "$1" = "pexa-cli" ] || [ "$1" = "pexa-tx" ]; then
  echo
  exec su-exec root "$@"
fi

echo
exec "$@"