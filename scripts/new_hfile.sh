#!/bin/sh


FILE=$1

[ -z "${FILE}" ] && exit 1;
[ -f "${FILE}" ] && exit 1;

BASENAME=$(basename ${FILE})
BASENAME_UP=$(echo $BASENAME | tr 'a-z.' 'A-Z_')

cat << END >"${FILE}"
/***************************************************************************
 *            ${BASENAME}
 *
 *  $(date -u) 
 *  Copyright $(date +%Y) Iuri Gomes Diniz
 *  \$Id\$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#ifndef _${BASENAME_UP}
#define _${BASENAME_UP}

#include <stdint.h> 

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */




#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _${BASENAME_UP} */
END
