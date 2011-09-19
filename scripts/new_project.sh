#!/bin/sh

#starts a new project with configure.ac and Makefile.am 



DEST="$1"

[ -z "${DEST}" ] && exit 1

[ -r "${DEST}" ] && exit 1



