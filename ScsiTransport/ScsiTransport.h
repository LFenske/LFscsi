#ifndef __scsibottom_h
#define __scsibottom_h

struct SCSITRANSPORT;

typedef struct SCSITRANSPORT *SCSI_HANDLE;

typedef enum DIRECTION {
  DIRECTION_NONE = 0,
  DIRECTION_OUT  = 1,
  DIRECTION_IN   = 2
} DIRECTION;

typedef enum RESET_LEVEL {
  RESET_DEVICE = 1,
  RESET_BUS    = 2
} RESET_LEVEL;

typedef int ScsiTransport_open(SCSI_HANDLE *pDevice, void *whatever);

typedef int ScsiTransport_close(SCSI_HANDLE *pDevice);

typedef int ScsiTransport_cdb(
                              SCSI_HANDLE device,
                              DIRECTION direction,
                              unsigned char *cdb, int  cdb_len ,
                              unsigned char *dat, int *dat_lenp,
                              unsigned char *stt, int *stt_lenp,
                              float timeout   /* seconds */
                              );

typedef int ScsiTransport_reset(SCSI_HANDLE device, RESET_LEVEL level);

typedef int ScsiTransport_scanbus(SCSI_HANDLE device);

ScsiTransport_open scsi_open;

typedef struct SCSITRANSPORT {
  ScsiTransport_close  * close  ;
  ScsiTransport_cdb    * cdb    ;
  ScsiTransport_reset  * reset  ;
  ScsiTransport_scanbus* scanbus;
  void                 * context;
} SCSITRANSPORT;

#endif /* __scsibottom_h */
