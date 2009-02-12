/*
 * Copyright (C) 2008  Larry Fenske
 * 
 * This file is part of LFscsi.
 * 
 * LFscsi is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * LFscsi is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with LFscsi.  If not, see <http://www.gnu.org/licenses/>.
 */
#define __common_c

#include "common.h"
#include <stdlib.h>   /* for malloc, free, NULL */


void
common_construct(COMMON_PARAMS *pCommon)
{
  *pCommon = malloc(sizeof(**pCommon));
  (*pCommon)->dat_size = NOSIZE;
  (*pCommon)->cdb_size = NOSIZE;
  (*pCommon)->flavor   = NOFLAVOR;
  (*pCommon)->timeout  = -1.;
  (*pCommon)->immed    = FALSE;
  (*pCommon)->verbose  = 0;
  (*pCommon)->stt.len  = 18;
  (*pCommon)->stt.dat  = malloc((*pCommon)->stt.len);
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


