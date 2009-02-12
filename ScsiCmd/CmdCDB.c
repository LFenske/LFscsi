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


#ifdef ENUM
  CMD_CDB ,
  CMD_CDBw,
  CMD_CDBr,
#endif


#ifdef DEF
  {CMD_CDB , "cdb" , LineCDB, DIRECTION_NONE, NULL, "-c cdb_size [<cdb bytes>]", ""},
  {CMD_CDBw, "cdbw", LineCDB, DIRECTION_OUT , NULL, "-c cdb_size -z data_size [<cdb bytes> [<data bytes>]]", ""},
  {CMD_CDBr, "cdbr", LineCDB, DIRECTION_IN  , NULL, "-c cdb_size -z data_size [<cdb bytes>]", ""},
#endif


#ifdef LINE
#include "CmdCDB.h"

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

int
LineCDB(SCSI_HANDLE handle, COMMON_PARAMS common,
	int argc, char**argv)
{
  VECTOR cdb;

  /* get CDB */
  cdb.len = common->cdb_size;
  cdb.dat = malloc(cdb.len);
  if (argc > optind) {
    int i;
    if (argc-optind < cdb.len) {
      fprintf(stderr, "not enough cdb, got %d, expected %d\n", argc-optind, cdb.len);
      free(cdb.dat);
      return(-2);
    }
    for (i=0; i<cdb.len; i++) {
      cdb.dat[i] = strtol(argv[optind++], (char**)NULL, 0);
    }
  } else {
    int bytesgotten = 0;
    while (bytesgotten < cdb.len) {
      int bytesthistime = read(0, cdb.dat+bytesgotten, cdb.len-bytesgotten);
      if (bytesthistime == 0) {
	fprintf(stderr, "not enough cdb, got %d, expected %d\n", bytesgotten, cdb.len);
	free(cdb.dat);
	return(-2);
      }
      bytesgotten += bytesthistime;
    }
  }

  /* get data */
  if (common->dir != DIRECTION_NONE) {
    dat.len = common->dat_size;
    dat.dat = malloc(dat.len);
  }
  if (common->dir == DIRECTION_OUT) {
    if (argc > optind) {
      int i;
      if (argc-optind < dat.len) {
	fprintf(stderr, "not enough data, got %d, expected %d\n", argc-optind, dat.len);
	free(dat.dat);
	return(-2);
      }
      for (i=0; i<dat.len; i++) {
	dat.dat[i] = strtol(argv[optind++], (char**)NULL, 0);
      }
    } else {
      int bytesgotten = 0;
      while (bytesgotten < dat.len) {
	int bytesthistime = read(0, dat.dat+bytesgotten, dat.len-bytesgotten);
	if (bytesthistime == 0) {
	  fprintf(stderr, "not enough data, got %d, expected %d\n", bytesgotten, dat.len);
	  free(dat.dat);
	  return(-2);
	}
	bytesgotten += bytesthistime;
      }
    }
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    /* this is unusual -- sending the CDB directly from here */
    send_cdb(handle, common,
	     common->dir,
	     cdb,
	     dat,
	     5.);
    free(cdb.dat);
  }
  return 0;
}
#endif


#if 0
#ifdef COMMAND
#define __CmdCDB_c
#include "CmdCDB.h"
#undef  __CmdCDB_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdCDB(SCSI_HANDLE handle, COMMON_PARAMS common,
           bool evpd, int page_code)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->size != NOSIZE) ? common->size : 0xff;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x??;
  cdb[1] = ;
  cdb[2] = ;
  cdb[3] = ;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_TEMPLATE,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif
#endif


