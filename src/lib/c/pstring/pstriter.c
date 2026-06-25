#include "pstring.h"

pstring* pstriter(const pstring *pstr) {
    return (pstring *) ((char *) pstr + pstrlen(pstr));
}
