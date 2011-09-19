#!/bin/sh
remove_ignore() {

    echo "cleaning '$PWD'"
    svn pget svn:ignore . 2>/dev/null| \
    while read i; do    
        rm -Rfv $i
    done

    find . -maxdepth 1 -not -name '.*' -and -type d | \
    while read i;do
        cd "$i"
        remove_ignore
        cd ..
    done
}



BASEDIR=$(dirname $0)"/../"

OLD_DIR=$OLDPWD

cd "$BASEDIR"

remove_ignore
cd $OLD_DIR

