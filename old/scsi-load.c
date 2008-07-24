int send_cdb(
             char *devfile,
             int direction,
             unsigned char *cdb, int  cdb_len ,
             unsigned char *dat, int *dat_lenp,
             unsigned char *stt, int *stt_lenp,
             int timeout);

typedef char bool;
#define FALSE 0
#define TRUE (!FALSE)

#include <unistd.h>   /* for getopt */
#include <stdlib.h>   /* for strtol, exit */
#include <limits.h>   /* for strtol */
#include <stdio.h>    /* for printf, fprintf */


void
usage(char *progname)
{
   fprintf(stderr, "usage: %s [-d device] [load/unload flag]\n", progname);
}


int
main(int argc, char**argv)
{
   char *progname = argv[0];
   char *device = getenv("DEVICE");
   bool load = TRUE;
   char cdb[ 6]; int cdb_len = sizeof(cdb);
   char stt[18]; int stt_len = sizeof(stt);
   char dat[ 1]; int dat_len = 0;
   int status;

   int ch;
   while ((ch = getopt(argc, argv, "d:")) != -1) {
      switch (ch) {
      case 'd':
         device = optarg;
         break;
      case '?':
      default:
         usage(progname);
         exit(-1);
      }
   }
   argc -= optind;
   argv += optind;

   if (argc > 0) {
      load = strtol(argv[0], (char**)NULL, 0);
      argc--;
      argv++;
   }

   if (argc > 0) {
      usage(progname);
      exit(-1);
   }

   cdb[0] = 0x1b;
   cdb[1] = 0;   /* bit 0: immediate */
   cdb[2] = 0;
   cdb[3] = 0;
   cdb[4] = (load ? 1 : 0);   /* bit 2: EOT; bit 1: Reten; bit 0: load */
   cdb[5] = 0;

   status = send_cdb(device, 'r',
                     cdb,  cdb_len,
                     dat, &dat_len,
                     stt, &stt_len,
                     120);

   if (status != 0) {
      printf("status = %d\n", status);
   }
   if ((status > 0) && (stt_len > 0)) {
      int i;
      printf("sense:");
      for (i=0; i<stt_len; i++)
         printf(" %.2x", (unsigned char)(stt[i]));
      printf("\n");
   }
}
