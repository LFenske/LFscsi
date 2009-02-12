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
#ifndef __common_h
#define __common_h

#include "ScsiTransport.h"

typedef  unsigned char  byte;

typedef char bool;
#define FALSE 0
#define TRUE (!FALSE)

typedef enum flavor {
  NOFLAVOR,
  SBC, /* SCSI Block Commands, e.g. disc */
  SSC, /* SCSI Stream Commands, e.g. tape */
  SMC, /* SCSI Media Changer Commands */
  FLAVOR_LAST
} FLAVOR;

typedef struct {
  byte *dat;
  int   len;
} VECTOR;

typedef void (*PRINTSUB)(VECTOR dat);

typedef struct {
  int    dat_size;
  int    cdb_size;
  FLAVOR flavor;   /* device-type, when it matters, e.g. READ command */
  float  timeout;
  bool   immed;
  int    verbose;
  VECTOR stt;
  int    cmd;      /* mapped from enum CMD */
  DIRECTION dir;
  PRINTSUB printer;
} *COMMON_PARAMS;

typedef struct {
  int key;
  char *data;
} tabletype;

#include "send_cdb.h"


void common_construct(COMMON_PARAMS *pCommon);
void  common_destruct(COMMON_PARAMS *pCommon);
char *lookup(tabletype *table, int value);

#endif /* __common_h */
