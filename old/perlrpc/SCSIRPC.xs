#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif



MODULE = SCSIRPC		PACKAGE = SCSIRPC

int
send_cdb(devfile, direction, cdb, dat, dat_len, stt, stt_len, timeout)
PREINIT:
    int		cdb_len;
    int		dat_len_dat;
INPUT:
    char*	devfile
    char	direction
    char*	cdb = (char *)SvPV(ST(2),cdb_len);
    char*	dat = (char *)SvPV(ST(3),dat_len_dat);
    int		dat_len
    char*	stt = NO_INIT
    int		stt_len
    int		timeout
INIT:
    bool	dat_malloced = FALSE;
CODE:
    switch (direction) {
    case 'w':
    case 'W':
    case '0':
	direction = 'w'; break;
    case 'r':
    case 'R':
    default:
	direction = 'r'; break;
    }
    if (direction == 'w'               ) {
        dat_len = dat_len_dat;
    }
    if (direction == 'r' && dat_len > 0) {
        dat = (char*) malloc(dat_len);
        dat_malloced = TRUE;
    }
    if (stt_len) stt = (char*) malloc(stt_len);
    else         stt = NULL;
    RETVAL = send_cdb(devfile, direction, cdb, cdb_len, dat, &dat_len, stt, &stt_len, timeout);
OUTPUT:
    RETVAL
    dat		if (direction == 'r' && dat_len) { sv_setpvn(ST(3), dat, dat_len); }
    stt		if (stt)                         { sv_setpvn(ST(5), stt, stt_len); }
CLEANUP:
    if (dat_malloced) free(dat);
    if (stt)          free(stt);
