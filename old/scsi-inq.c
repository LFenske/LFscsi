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
   fprintf(stderr, "usage: %s [-d device] [-z size] [page code]\n", progname);
}


int
main(int argc, char**argv)
{
   char *progname = argv[0];
   char *device = getenv("DEVICE");
   char cdb[  6]; int cdb_len = sizeof(cdb);
   char stt[ 18]; int stt_len = sizeof(stt);
   char dat[255]; int dat_len = 0;
   int status;

   int size = 0xff;
   int page = -1;

   int ch;
   while ((ch = getopt(argc, argv, "d:z:")) != -1) {
      switch (ch) {
      case 'd':
         device = optarg;
         break;
      case 'z':
         size = strtol(optarg, (char**)NULL, 0);
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
      page = strtol(argv[0], (char**)NULL, 0);
      argc--;
      argv++;
   }

   if (argc > 0) {
      usage(progname);
      exit(-1);
   }

   cdb[0] = 0x12;
   cdb[1] = (page == -1) ? 0 : 1;
   cdb[2] = (page == -1) ? 0 : page;
   cdb[3] = 0;
   cdb[4] = size;
   cdb[5] = 0;

   dat_len = size;
   status = send_cdb(device, 'r',
                     cdb,  cdb_len,
                     dat, &dat_len,
                     stt, &stt_len,
                     5);

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
   if (1) {
      int i;
      printf("inquiry data:");
      for (i=0; i<dat_len; i++)
         printf(" %.2x", (unsigned char)(dat[i]));
      printf("\n");
   }
}
