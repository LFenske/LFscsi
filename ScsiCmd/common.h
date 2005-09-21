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
  bool immed;
  VECTOR stt;
} *COMMON_PARAMS;

typedef struct {
  int key;
  char *data;
} tabletype;

#include "send_cdb.h"


void common_construct(COMMON_PARAMS *pCommon);
void  common_destruct(COMMON_PARAMS *pCommon);
char *lookup(tabletype *table, int value);

#endif /* __common_h */
