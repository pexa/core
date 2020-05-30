#!/usr/bin/env bash
# Copyright (c) 2016-2019 The Pexa Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C
TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
BUILDDIR=${BUILDDIR:-$TOPDIR}

BINDIR=${BINDIR:-$BUILDDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

PEXAD=${PEXAD:-$BINDIR/pexad}
PEXACLI=${PEXACLI:-$BINDIR/pexa-cli}
PEXATX=${PEXATX:-$BINDIR/pexa-tx}
WALLET_TOOL=${WALLET_TOOL:-$BINDIR/pexa-wallet}
PEXAQT=${PEXAQT:-$BINDIR/qt/pexa-qt}

[ ! -x $PEXAD ] && echo "$PEXAD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
read -r -a PEXAVER <<< "$($PEXACLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }')"

# Create a footer file with copyright content.
# This gets autodetected fine for pexad if --version-string is not set,
# but has different outcomes for pexa-qt and pexa-cli.
echo "[COPYRIGHT]" > footer.h2m
$PEXAD --version | sed -n '1!p' >> footer.h2m

for cmd in $PEXAD $PEXACLI $PEXATX $WALLET_TOOL $PEXAQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${PEXAVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${PEXAVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
