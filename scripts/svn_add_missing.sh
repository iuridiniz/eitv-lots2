#!/bin/sh


svn st | grep '^? ' | perl -pe 's/^\?\s+//' | xargs svn add -N 

