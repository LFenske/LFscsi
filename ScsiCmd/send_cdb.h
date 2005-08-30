#ifndef __send_cdb_h
#define __send_cdb_h

#include "common.h"
#include "ScsiTransport.h"

#define NOSIZE  (-1)

#ifdef __send_cdb_c
#define GLOBAL
#else
#define GLOBAL extern
#endif


void send_cdb(SCSI_HANDLE device,
              COMMON_PARAMS common,
	      DIRECTION dir,
	      byte *cdb, int cdb_len,
	      VECTOR dat,
	      float default_timeout);


#endif /* __send_cdb_h */
