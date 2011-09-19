#!/bin/sh


FILE=$1

[ -z "${FILE}" ] && exit 1;
[ -f "${FILE}" ] && exit 1;

BASENAME=$(basename "${FILE}")
HEADER=$(basename "${FILE}" ".c")".h"

cat << END >"${FILE}"
/***************************************************************************
 *            ${BASENAME}
 *
 *  $(date -u) 
 *  Copyright $(date +%Y) Iuri Gomes Diniz
 *  \$Id\$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "${HEADER}"

void __func(void) 
{

}

END
