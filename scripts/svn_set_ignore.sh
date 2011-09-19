#!/bin/sh

BASEDIR=`dirname $0`"/.."
FILE=`dirname $0`"/files_to_ignore.txt"

svn pset svn:ignore -R -F "${FILE}" $BASEDIR

