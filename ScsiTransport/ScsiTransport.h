/*
 * Copyright (C) 2008  Larry Fenske
 * 
 * This file is part of LFscsi.
 * 
 * LFscsi is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * LFscsi is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with LFscsi.  If not, see <http://www.gnu.org/licenses/>.
 */
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
