#ifndef __common_h
#define __common_h

typedef  unsigned char  byte;

typedef char bool;
#define FALSE 0
#define TRUE (!FALSE)

typedef struct {
  byte *dat;
  int   len;
} VECTOR;

typedef struct {
  int size;
  float timeout;
  VECTOR stt;
} *COMMON_PARAMS;

#include "send_cdb.h"

#endif /* __common_h */
