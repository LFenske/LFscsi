#define __common_c

#include "common.h"
#include <stdlib.h>   /* for malloc, free, NULL */


void
common_construct(COMMON_PARAMS *pCommon)
{
  *pCommon = malloc(sizeof(**pCommon));
  (*pCommon)->size    = NOSIZE;
  (*pCommon)->timeout = -1.;
  (*pCommon)->immed   = FALSE;
  (*pCommon)->stt.len = 18;
  (*pCommon)->stt.dat = malloc((*pCommon)->stt.len);
}


void
common_destruct(COMMON_PARAMS *pCommon)
{
  free((*pCommon)->stt.dat);
  free(*pCommon); *pCommon = NULL;
}


char *
lookup(tabletype *table, int value)
{
  while (table->key != value && table->data != NULL)
    table++;
#if 0
  if (table->data == NULL)
    return "unknown";
  else
#endif
    return table->data;
}


