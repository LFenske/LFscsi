#ifndef __common_h
#define __common_h

#include "ScsiTransport.h"

typedef  unsigned char  byte;

typedef char bool;
#define FALSE 0
#define TRUE (!FALSE)

typedef enum flavor {
  NOFLAVOR,
  SBC, /* SCSI Block Commands, e.g. disc */
  SSC, /* SCSI Stream Commands, e.g. tape */
  SMC, /* SCSI Media Changer Commands */
  FLAVOR_LAST
} FLAVOR;

typedef struct {
  byte *dat;
  int   len;
} VECTOR;

typedef struct {
  int    dat_size;
  int    cdb_size;
  FLAVOR flavor;   /* device-type, when it matters, e.g. READ command */
  float  timeout;
  bool   immed;
  int    verbose;
  VECTOR stt;
  int    cmd;      /* mapped from enum CMD */
  DIRECTION dir;
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
