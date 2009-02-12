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
/* -*- mode: c -*- */


#include <windows.h>
#include <sys/types.h>   // Hopefully has the 64bit long numbers support
#include <ddk/ntddscsi.h>
#include <stdio.h>
#include <stdlib.h>
#include "ScsiTransport.h"


#define debug 0
#define MAX_NUM_LIBRARIES 8
#define SENSE_DATA_SIZE   52


typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER {
    SCSI_PASS_THROUGH_DIRECT sptd;
    ULONG             Filler;      // realign buffer to double word boundary
    UCHAR             ucSenseBuf[SENSE_DATA_SIZE];
} SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;


static int
scsi_close(SCSI_HANDLE *pDevice)
{
  CloseHandle((*pDevice)->context);
  free(*pDevice);
  *pDevice = NULL;
  return 0;
}


static int
scsi_cdb(
         SCSI_HANDLE device,
         DIRECTION direction,
         unsigned char *cdb, int  cdb_len ,
         unsigned char *dat, int *dat_lenp,
         unsigned char *stt, int *stt_lenp,
         float timeout)
{
  int         retval = 0,
              MSdirection;
  ULONG       ulLength,
              ul_ReturnValue=0;

  switch (direction) {
      // Microsoft Data Direction is DIFFERENT than what is specified for DIRECTION ENUM
      //#define SCSI_IOCTL_DATA_OUT               0
      //#define SCSI_IOCTL_DATA_IN                1
      //#define SCSI_IOCTL_DATA_UNSPECIFIED       2
      case DIRECTION_NONE:  MSdirection = SCSI_IOCTL_DATA_UNSPECIFIED; break;
      case DIRECTION_IN  :  MSdirection = SCSI_IOCTL_DATA_IN;          break;
      case DIRECTION_OUT :  MSdirection = SCSI_IOCTL_DATA_OUT;         break;
  }
  if (debug) {
    fprintf(stderr, "cdb = 0x%.8lx, cdb_len =  %d\n", (long)cdb, cdb_len );
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)dat,*dat_lenp);
    fprintf(stderr, "stt = 0x%.8lx, stt_len =  %d\n", (long)stt,*stt_lenp);
    fprintf(stderr, "timeout = %f\n", timeout);
    fprintf(stderr, "direction = %d, MSdirection = %d\n", direction, MSdirection);
  }
  if (debug) {
    int i;
    fprintf(stderr, "SCSI CDB:\n");
    for (i=0; i<cdb_len; i++)
      fprintf(stderr, " %02x", cdb[i]);
    fprintf(stderr, "\n");
  }
  if (direction == DIRECTION_OUT && debug) {
    int i;
    fprintf(stderr, "SCSI data to write:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }

  if (debug>=2) {
      //fprintf(stderr"Bus Num                %d\n", unBusNumber);
      //fprintf(stderr"Target ID is           %d\n", unTarget);
      //fprintf(stderr"LUN ID is              %d\n", unLun);
      fprintf(stderr,"Data IO Directions is  ");
      switch (direction) {
      case DIRECTION_OUT:
          fprintf(stderr,"OUT - (From Host to Device)\n");
          break;
      case DIRECTION_IN:
          fprintf(stderr,"IN - (From Device to Host)\n");
          break;
      case DIRECTION_NONE:
          fprintf(stderr,"None - (No Data transfer)\n");
          break;
      }
      fprintf(stderr,"CDB Length is          %d bytes\n", cdb_len);
      fprintf(stderr,"Length of expected IO  %d bytes\n", *dat_lenp);
      fprintf(stderr,"SCSI Timeout value     %lu seconds\n", (ULONG)timeout);
  }
  {
    /*stub*/
      int         ii = 0;
      SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;

      ZeroMemory(&sptdwb,sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER));

      sptdwb.sptd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT); //+SPT_SENSE_DATA_SIZE;
      //sptdwb.sptd.PathId = unBusNumber;
      sptdwb.sptd.PathId = 0;
      //sptdwb.sptd.TargetId = unTarget;
      sptdwb.sptd.TargetId = 0;
      //sptdwb.sptd.Lun = unLun;
      sptdwb.sptd.Lun = 0;
      sptdwb.sptd.CdbLength = cdb_len;
      sptdwb.sptd.SenseInfoLength = *stt_lenp;
      sptdwb.sptd.DataIn = MSdirection;

      if (MSdirection == DIRECTION_NONE) {
          sptdwb.sptd.DataTransferLength = 0;
      }else {
          sptdwb.sptd.DataTransferLength = *dat_lenp;
      }

      sptdwb.sptd.TimeOutValue = (unsigned long)timeout;
      /*if (nDebugPrint >= 2) {
            printf("*****   ExecSCSICommand  Cmd Timeout = %lu   *****\n",
                    sptdwb.sptd.TimeOutValue);
      }
      */
      sptdwb.sptd.DataBuffer = dat;

      sptdwb.sptd.SenseInfoOffset =
         offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER,ucSenseBuf);

      for (ii=0; ii< cdb_len; ii++) {
          sptdwb.sptd.Cdb[ii] = cdb[ii];
      }
      if (debug) {
          fprintf(stderr,"Sending CDB to Device (HEX):  ");
          for (ii=0; ii< cdb_len; ii++) {
              fprintf(stderr,"%0X ",  sptdwb.sptd.Cdb[ii]);
          }
          fprintf(stderr,"\n");

          if ((debug >= 2) && (direction == DIRECTION_OUT)) {
              // UINT        jj = 0;
              fprintf(stderr,"Sending Data Out (HEX):  \n");
             /* for (jj=0; jj < dat_lenp ; jj++) {
                  printf("%x ", pDataBuffer[jj]);
              }
              printf("\n");
              */
          }
      }

      ulLength = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER)+ sptdwb.sptd.SenseInfoLength;

      retval = DeviceIoControl(device->context,
                                IOCTL_SCSI_PASS_THROUGH_DIRECT,
                                &sptdwb,
                                ulLength,
                                &sptdwb,
                                ulLength,
                                &ul_ReturnValue,
                                FALSE);

      //ScsiCmdStatus.ScsiStatus = sptdwb.sptd.ScsiStatus;
      //ScsiCmdStatus.SenseInfoLength = sptdwb.sptd.SenseInfoLength;

      if (!retval)
      {
         LPVOID         errorBuffer;
         unsigned long  ulCount;
         DWORD          dwERRorCode;

         fprintf(stderr, "Error in DeviceIoControl of scsi_cdb!; Windows Error = %lu\n",
                      dwERRorCode = GetLastError());

         ulCount = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                 NULL,
                                 dwERRorCode,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 0,
                                 (LPTSTR) &errorBuffer, // errorBuffer,
                                 sizeof(errorBuffer), // 0,
                                 NULL );

         if (ulCount != 0)
             fprintf(stderr,"%s", (char*)errorBuffer);
         else
             fprintf(stderr,"Format error message failed.  Error: %lu\n", GetLastError());

         free(errorBuffer);

         return -1;
      }

      if (debug) {
         fprintf(stderr," Returned Scsi_Status = %u\n",sptdwb.sptd.ScsiStatus);
         if (sptdwb.sptd.ScsiStatus != 0) {
             fprintf(stderr," Sense Info Length    = %u\n",sptdwb.sptd.SenseInfoLength);
         }
      }

      if (debug && (direction == DIRECTION_IN)) {
          fprintf(stderr," Data length ReturnValue is %lu\n", sptdwb.sptd.DataTransferLength);
      }

      if (debug) {
          fprintf(stderr,"DeviceIoControl Command Status = %d\n", retval);
          fprintf(stderr,"DeviceIoControl ReturnValue    = %lu\n", ul_ReturnValue);
      }

      if (direction != DIRECTION_NONE) {
          *dat_lenp = sptdwb.sptd.DataTransferLength;
      } else {
          *dat_lenp = 0;
      }

      if (!(sptdwb.sptd.ScsiStatus))
      {
          /*if (!bSCSIDev) {
              unBusNumber = sptdwb.sptd.PathId;
              unTarget = sptdwb.sptd.TargetId;
              unLun = sptdwb.sptd.Lun;
          }  */
          *stt_lenp = 0;

      } else {
          int        jj = 0;
          for(jj=0; jj< *stt_lenp; jj++)
              stt[jj] = sptdwb.ucSenseBuf[jj];
          //return  sptdwb.sptd.ScsiStatus;
      }
      retval = sptdwb.sptd.ScsiStatus;
  }

  if (direction == DIRECTION_IN && debug && retval != -1) {
    int i;
    fprintf(stderr, "SCSI data read:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }

  if (stt[0] != 0 || retval == -1) {
    if (*stt_lenp > 0) {
      if (debug) {
    int i;
    fprintf(stderr, "SCSI request sense:\n");
    for (i=0; i<*stt_lenp; i++)
      fprintf(stderr, " %02x", stt[i]);
    fprintf(stderr, "\n");
      }
    }
  }
  if (debug) fprintf(stderr, "retval = %d\n", retval);
  return(retval);
}


static int
scsi_reset(SCSI_HANDLE device, RESET_LEVEL level)
{
  switch (level) {
  case RESET_DEVICE: /*stub*/ break;
  case RESET_BUS   : /*stub*/ break;
  }
  return 0;
}


static int
scsi_scanbus(SCSI_HANDLE device)
{
    int                     bStatus = 0;
    PSCSI_ADAPTER_BUS_INFO  adapterInfo;
    PSCSI_INQUIRY_DATA      inquiryData;
    unsigned int            i,
                            j;
    unsigned int            un_Buffersize = 2048,
                            NumberOfDevices = 0;
    unsigned char           *sBuffer;
    unsigned long           ul_ReturnValue = 0;
    //DWORD                   dwERRorCode = 0;
    //S_LIBRARY_INFO          Library_info[MAX_NUM_LIBRARIES];

//    UCHAR NumberOfDevices = 0;
//    UCHAR NumberOfLibraries = 0;


    sBuffer = malloc(un_Buffersize);
    if (sBuffer == NULL)
    {
         printf ("Error in obtaining buffer space \n");
         return -1;
    }

    // Zero out the Buffer
    memset(sBuffer,0,un_Buffersize);

    bStatus = DeviceIoControl(device->context,
                         IOCTL_SCSI_GET_INQUIRY_DATA,
                         NULL,
                         0,
                         sBuffer,
                         un_Buffersize, //sizeof(sBuffer),
                         &ul_ReturnValue,
                         FALSE);

    if (!bStatus)
    {
        //if (debug) {
            LPVOID         errorBuffer;
            unsigned long  ulCount;
            DWORD          dwERRorCode = GetLastError();
            //printf("Error in Scanning SCSI BUS %s\n", csDevFile);
            //printf("Error was %lu\n",

            ulCount = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                    NULL,
                                    dwERRorCode,
                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 0,
                                    (LPTSTR) &errorBuffer, // errorBuffer,
                                    sizeof(errorBuffer), // 0,
                                    NULL );

            if (ulCount != 0)
                printf("%s", (char*)errorBuffer);
            else
                printf("Format error message failed.  Error: %lu\n", GetLastError());

            free(errorBuffer);
        //}
        free(sBuffer);
        //bDone = TRUE;
        return -1;
    }

    adapterInfo = (PSCSI_ADAPTER_BUS_INFO) sBuffer;

    //memset (Library_info, 0, (sizeof (S_LIBRARY_INFO)*8) );
    //display = 1;
    //if (display) {
        printf("Bus\n");
        printf("Num TID LUN Claimed String                        Inquiry Header\n");
        printf("--- --- --- ------- ----------------------------  -----------------------\n");
    //}

    //memset (Library_info, 0, (sizeof (S_LIBRARY_INFO)*8) );
    for (i = 0; i < adapterInfo->NumberOfBuses; i++)
    {
        inquiryData = (PSCSI_INQUIRY_DATA) (sBuffer +
                       adapterInfo->BusData[i].InquiryDataOffset);

        while (adapterInfo->BusData[i].InquiryDataOffset)
        {
            NumberOfDevices++;

         /*   if ((inquiryData->InquiryData[0] & 0x1F) == 8)
            {
                //NumberOfLibraries++;

            } */
            printf(" %d  %03d  %02d    %s    %.28s  ",
                            inquiryData->PathId,
                            inquiryData->TargetId,
                            inquiryData->Lun,
                            (inquiryData->DeviceClaimed) ? "Y" : "N",
                            &inquiryData->InquiryData[8]);

            for (j = 0; j < 8; j++)
                printf("%02X ", inquiryData->InquiryData[j]);

            printf("\n");

            if (inquiryData->NextInquiryDataOffset == 0)
                break;
            if (NumberOfDevices >= 64)
                break;

            inquiryData = (PSCSI_INQUIRY_DATA) (sBuffer +
                           inquiryData->NextInquiryDataOffset);
        }
    }
    printf("\n");
   // printf ("Number of devices:   %d\n", NumberOfDevices);
   // printf ("Number of Libraries: %d\n", NumberOfLibraries);
   // printf("\n");

    free(&adapterInfo);
    free(&inquiryData);
    free(sBuffer);

    return 0;
}


int
scsi_open(SCSI_HANDLE *pDevice, void *whatever)
{
  HANDLE pfd;
  char   NTDevice[80];

  if (debug) fprintf(stderr, "scsi_open '%s'\n", (char*)whatever);
  *pDevice = malloc(sizeof(**pDevice));

  strcpy(NTDevice,"\\\\.\\");
  strcat(NTDevice,whatever);

  pfd = CreateFile(NTDevice, GENERIC_WRITE | GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL, OPEN_EXISTING, 0, NULL);

  if ((pfd == INVALID_HANDLE_VALUE) || (pfd == NULL))
  {
    LPVOID         errorBuffer;
    unsigned long  ulCount;
    DWORD          dwERRorCode = GetLastError();

    ulCount = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL,
                            dwERRorCode,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 0,
                            (LPTSTR) &errorBuffer, // errorBuffer,
                            sizeof(errorBuffer), // 0,
                            NULL );

    if (ulCount != 0)
      printf("%s", (char*)errorBuffer);
    else
      printf("Format error message failed.  Error: %lu\n", GetLastError());

    free(errorBuffer);
    return (-1);
  }
  (*pDevice)->close   = scsi_close  ;
  (*pDevice)->cdb     = scsi_cdb    ;
  (*pDevice)->reset   = scsi_reset  ;
  (*pDevice)->scanbus = scsi_scanbus;
  (*pDevice)->context = pfd;
  return 0;
}


