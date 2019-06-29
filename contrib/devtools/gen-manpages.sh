#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

PEXAD=${PEXAD:-$SRCDIR/pexad}
PEXACLI=${PEXACLI:-$SRCDIR/pexa-cli}
PEXATX=${PEXATX:-$SRCDIR/pexa-tx}
PEXAQT=${PEXAQT:-$SRCDIR/qt/pexa-qt}

[ ! -x $PEXAD ] && echo "$PEXAD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
PEXAVER=($($PEXACLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for pexad if --version-string is not set,
# but has different outcomes for pexa-qt and pexa-cli.
echo "[COPYRIGHT]" > footer.h2m
$PEXAD --version | sed -n '1!p' >> footer.h2m

for cmd in $PEXAD $PEXACLI $PEXATX $PEXAQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${PEXAVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${PEXAVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
