#include "/usr/src/linux/include/scsi/sg.h"
#include <sys/types.h>	/* for open */
#include <sys/stat.h>	/* for open */
#include <fcntl.h>	/* for open */

main()
{
  char buffer[100], *bp;
  struct sg_header *headerp = (struct sg_header*) buffer;
  int fd;
  int res;
  int i;

  fd = open("/dev/sga", O_RDWR);
  if (fd == -1) {
    perror("device open");
    exit(-1);
  }
  ioctl(fd, SG_SET_TIMEOUT, 5*100);
  headerp->pack_len    = sizeof(*headerp)+6;
  headerp->reply_len   = sizeof(*headerp)+32;
  headerp->pack_id     = 0;
  headerp->result      = -1;
  headerp->twelve_byte = 0;
  headerp->other_flags = 0;
  memset(headerp->sense_buffer, 0, sizeof(headerp->sense_buffer));
  bp = buffer + sizeof(*headerp);
  *(bp++) = 0x1a;
  *(bp++) = 0x08;
  *(bp++) = 0x3f;
  *(bp++) = 0x00;
  *(bp++) = 32;
  *(bp++) = 0x00;

  {
    int i;
    printf("header out:\n");
    for (i=0; i<sizeof(*headerp); i++)
      printf(" %.2x", buffer[i]&0xff);
    printf("\n");
    printf("cdb and data out:\n");
    for (i=sizeof(*headerp); i<bp-buffer; i++)
      printf(" %.2x", buffer[i]&0xff);
    printf("\n");
  }

  res = write(fd, buffer, bp-buffer);
  if (res != bp-buffer) {
    printf("write result is %d, should be %d\n", res, bp-buffer);
  }
  res = read(fd, buffer, sizeof(buffer));
  printf("read result is %d\n", res);
  printf("pack_len  = %d\n", headerp->pack_len );
  printf("reply_len = %d\n", headerp->reply_len);
  printf("pack_id   = %d\n", headerp->pack_id  );
  printf("result    = %d\n", headerp->result   );
  printf("sense_buffer:\n");
  for (i=0; i<sizeof(headerp->sense_buffer); i++) {
    printf(" %.2x", headerp->sense_buffer[i]&0xff);
  }
  printf("\n");
  if (res > sizeof(*headerp)) {
    printf("data in:\n");
    for (i=sizeof(*headerp); i<res; i++) {
      printf(" %.2x", buffer[i]&0xff);
    }
    printf("\n");
  }
}


