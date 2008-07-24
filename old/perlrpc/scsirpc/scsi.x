/* -*- mode: c -*- */

/*
 * scsi.x - Specification of remote SCSI service.
 */

typedef char byte;

struct send_cdb_params {
  string device<>;
  char   rw;
  opaque cdb<12>;
  opaque dat<>;
  int    dat_maxlen;
  int    stt_maxlen;
  int    timeout;
};

struct send_cdb_retval {
  byte   status;
  opaque dat<>;
  opaque stt<>;
};

program SCSI_PROG {
    version SCSI_VERS {
	send_cdb_retval SEND_CDB(send_cdb_params) = 1;
	int RESET   (string)          = 2;
    } = 1;				/* version number */
} = 600039074;			/* program number */
					/* 0x20000000 - 0x3fffffff for users*/
