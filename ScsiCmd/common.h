#ifndef __common_h
#define __common_h

typedef  unsigned char  byte;

typedef char bool;
#define FALSE 0
#define TRUE (!FALSE)

typedef struct {
  byte *dat;
  int   len;
} vector;

#include "send_cdb.h"

#endif /* __common_h */
