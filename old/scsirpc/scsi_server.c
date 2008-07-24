#include <stdio.h>
#include <rpc/rpc.h>
#include "scsi_generic.h"	/* includes scsi.h */

void dump(char *val, u_int len, char *label);

#ifdef linux
send_cdb_retval * send_cdb_1_svc(send_cdb_params *params, struct svc_req *svc)
#else
send_cdb_retval * send_cdb_1    (send_cdb_params *params)
#endif
{
  static send_cdb_retval *retval;

  /*
  dump(params->dat.dat_val, params->dat.dat_len, "odat");
  */
  retval = send_cdb(params);
  /*
  dump(retval->dat.dat_val, retval->dat.dat_len, "idat");
  */
  return retval;
}

#ifdef linux
int * reset_1_svc(char **device, struct svc_req *svc)
#else
int * reset_1    (char **device)
#endif
{
  printf("RESET\n");
  printf("device:  %s\n", *device);
  printf("\n");
  return 0;
}

void dump(char *val, u_int len, char *label)
{
  int i;
  printf("%s:\n", label);
  for (i=0; i<len; i++) {
    if (i%16 == 0)
      printf(" ");
    printf(" %.2x", (unsigned char)val[i]);
    if ((i+1)%16 == 0)
      printf("\n");
  }
  if (i%16 != 0)
    printf("\n");
}

