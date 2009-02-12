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
#define __PrintModeSenseSub_c

#include "common.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy, strcat */
#include <ctype.h>    /* for isprint */


#define MIN(__a, __b)  ((__a) < (__b) ? (__a) : (__b))


static VECTOR myread_data;


static void
myread_init(VECTOR dat)
{
  myread_data = dat;
}


static int
myread(byte *pC, int len)
{
  int retval;
  retval = len;
  if (retval > myread_data.len)
    retval = myread_data.len;
  memcpy(pC, myread_data.dat, retval);
  myread_data.dat += retval;
  myread_data.len -= retval;
  return retval;
}


byte *
myread_ptr()
{
  return myread_data.dat;
}


char *
caps(int bits)
{
  static char res[4*3+1];
  res[0] = '\0';
  strcat(res, "  "); strcat(res, (bits & 8 ? "X" : " "));
  strcat(res, "  "); strcat(res, (bits & 4 ? "X" : " "));
  strcat(res, "  "); strcat(res, (bits & 2 ? "X" : " "));
  strcat(res, "  "); strcat(res, (bits & 1 ? "X" : " "));
  return res;
}


char *
ED(int bit)
{
    return bit ? "ENABLED" : "DISABLED";
}


void
PrintModeSenseSub(VECTOR dat, bool bighead)
{
  byte $density, $type;
  char *$ps;
  int $spf;
  byte qc[16];
  int $totlen;
  byte $medtype, $WPcache, $BDL;

  if (dat.dat == NULL)
    return;

  myread_init(dat);

  if (bighead) {
     if (8 != myread(qc, 8))
        return;
     $totlen  = (qc[0] << 8) | (qc[1] << 0);
     $medtype = qc[2];
     $WPcache = qc[3];
     $BDL     = (qc[6] << 8) | (qc[7] << 0);
     $totlen -= 6;
  } else {
     if (4 != myread(qc, 4))
        return;
     $totlen  = qc[0];
     $medtype = qc[1];
     $WPcache = qc[2];
     $BDL     = qc[3];
     $totlen -= 3;
  }
  if ($BDL) {
    int $numblocks, $blocklen;
    byte *$q;
    $q = malloc($BDL);
    if ($BDL != myread($q, $BDL)) {
      free($q);
      return;
    }
    $density   =  $q[0];
    $numblocks = ($q[1] << 16) | ($q[2] <<  8) | ($q[3] <<  0);
    $blocklen  = ($q[5] << 16) | ($q[6] <<  8) | ($q[7] <<  0);
    $totlen -= $BDL;
    printf("Block Descriptor\n");
    if ($density) {
      printf("  Density: %s",
             $density == 0x03 ? "Optical Erasable 650 MB\n" :
             $density == 0x06 ? "Optical Write-Once 650 MB\n" :
             $density == 0x0a ? "Optical 1.3 GB\n" :
             "unknown\n");
    }
    printf("  %7d Blocks of\n", $numblocks);
    printf("  %7d bytes each\n", $blocklen);
    free($q);
  }
  while ($totlen > 0) {
    byte $q[2];
    int headsize;
    byte *$page;
    byte subpage = -1;
    char subpagestr[4] = "";
    byte $rawtype, $len;
    byte *datastart = myread_ptr();

    if (2 != myread($q, 2))
      return;

    $rawtype = $q[0];
    $ps   = ($rawtype >> 7) & 1 ? "savable" : "not savable";	/* top bit */
    $spf  = ($rawtype >> 6) & 1;
    $type = ($rawtype >> 0) & 0x3f;	/* bottom six bits */
    if ($spf) {
      subpage = $q[1];
      if (2 != myread($q, 2))
        return;
      sprintf(subpagestr, "-%.2x", subpage);
      $len    = ($q[0] << 8) | ($q[1] << 0);
      $totlen -= $len+4;
    } else {
      $len     = $q[1];
      $totlen -= $len+2;
    }
    headsize = myread_ptr()-datastart;
    $page = malloc($len);
    if ($len != myread($page, $len)) {
      free($page);
      return;
    }

    switch (($type << 8) | (subpage & 0xff)) {
    case 0x1dff:
      {
        short $MTad, $MTnm, $STad, $STnm, $IEad, $IEnm, $DTad, $DTnm;
        byte *pp = $page;
        $MTad = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        $MTnm = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        $STad = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        $STnm = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        $IEad = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        $IEnm = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        $DTad = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        $DTnm = (pp[0] << 8) | (pp[1] << 0); pp += 2;
        printf("page 0x1d: Element Address Assignment (%s)\n", $ps);
        printf("      start num\n");
        printf("     ------ ---\n");
        printf("  MT 0x%.4x %d\n", $MTad, $MTnm);
        printf("  ST 0x%.4x %d\n", $STad, $STnm);
        printf("  IE 0x%.4x %d\n", $IEad, $IEnm);
        printf("  DT 0x%.4x %d\n", $DTad, $DTnm);
      }
    case 0x1eff:
      {
        int $bytenum;
        byte $rotate, $transport;
        printf("page 0x1e: Transport Geometry (%s)\n", $ps);
        for ($bytenum=0; $bytenum < $len; $bytenum += 2) {
          $rotate    = $page[$bytenum + 0];
          $transport = $page[$bytenum + 1];
          printf("  Transport %d is %s\n", $transport, $rotate&1 ? "rotatable" : "non-rotatable");
        }
      }
    case 0x1fff:
      {
        byte $stor, $frMT, $frST, $frIE, $frDT, $exMT, $exST, $exIE, $exDT;
        printf("page 0x1f: Device Capabilities (%s)\n", $ps);
        $stor = $page[ 0];
        $frMT = $page[ 2];
        $frST = $page[ 3];
        $frIE = $page[ 4];
        $frDT = $page[ 5];
        $exMT = $page[ 8];
        $exST = $page[ 9];
        $exIE = $page[10];
        $exDT = $page[11];
        printf("  op & source  DT IE ST MT\n");
        printf("  ------------ -- -- -- --\n");
        printf("  store       %s\n", caps($stor));
        printf("  move from MT%s\n", caps($frMT));
        printf("  move from ST%s\n", caps($frST));
        printf("  move from IE%s\n", caps($frIE));
        printf("  move from DT%s\n", caps($frDT));
        printf("  exch MT with%s\n", caps($exMT));
        printf("  exch ST with%s\n", caps($exST));
        printf("  exch IE with%s\n", caps($exIE));
        printf("  exch DT with%s\n", caps($exDT));
      }
    case 0x30ff:
      {
        printf("page 0x30: Additional Delay (%s)\n", $ps);

        if ($spf) {
          byte fulldata[12];
          int delay    ;
          int countnext;
          int period   ;
          int countleft;
          byte *pp = fulldata;
          memset(fulldata, 0, sizeof(fulldata));
          memcpy(fulldata, $page, MIN(sizeof(fulldata), $len));
          delay     = (pp[0] << 16) | (pp[1] << 8) | (pp[2] << 0); pp+=3;
          countnext = (pp[0] << 16) | (pp[1] << 8) | (pp[2] << 0); pp+=3;
          period    = (pp[0] << 16) | (pp[1] << 8) | (pp[2] << 0); pp+=3;
          countleft = (pp[0] << 16) | (pp[1] << 8) | (pp[2] << 0); pp+=3;
          printf("subpage %.2x %7d %7d %7d %7d\n", subpage, delay, countnext, period, countleft);
        } else {
          printf("illegal SPF == 0\n");
          break;
        }
      }
      break;
    case 0x08ff:
      {
	printf("page 0x08: Caching mode page (%s)\n", $ps);
	printf("  IC    : %x\n", ($page[ 0]>>7)&1);
	printf("  ABPF  : %x\n", ($page[ 0]>>6)&1);
	printf("  CAP   : %x\n", ($page[ 0]>>5)&1);
	printf("  DISC  : %x\n", ($page[ 0]>>4)&1);
	printf("  SIZE  : %x\n", ($page[ 0]>>3)&1);
	printf("  WCE   : %x\n", ($page[ 0]>>2)&1);
	printf("  MF    : %x\n", ($page[ 0]>>1)&1);
	printf("  RCD   : %x\n", ($page[ 0]>>0)&1);
	printf("  retention priority, demand read/write: %d/%d\n", ($page[ 1]>>4)&0xf, ($page[ 1]>>0)&0xf);
	printf("  disable pre-fetch transfer length: 0x%.4x\n", ($page[ 2]<<8)|($page[ 3]<<0));
	printf("  minimum pre-fetch                : 0x%.4x\n", ($page[ 4]<<8)|($page[ 5]<<0));
	printf("  maximum pre-fetch                : 0x%.4x\n", ($page[ 6]<<8)|($page[ 7]<<0));
	printf("  maximum pre-fetch ceiling        : 0x%.4x\n", ($page[ 8]<<8)|($page[ 9]<<0));
	printf("  FSW   : %x\n", ($page[10]>>7)&1);
	printf("  LBCSS : %x\n", ($page[10]>>6)&1);
	printf("  DRA   : %x\n", ($page[10]>>5)&1);
	printf("  ??    : %x\n", ($page[10]>>3)&3);
	printf("  NV_DIS: %x\n", ($page[10]>>0)&1);
	printf("  number of cache segments: 0x%.2x\n", $page[11]);
	printf("  cache segment size: 0x%.4x\n", ($page[12]<<8)|($page[13]<<0));
      }
      break;
    case 0x0aff:
      {
	printf("page 0x0a: Control mode page (%s)\n", $ps);
	printf("  tst            : %x\n", ($page[0]>>5)&0x7);
	printf("  tmf_only       : %x\n", ($page[0]>>4)&0x1);
	printf("  D_Sense        : %x\n", ($page[0]>>2)&0x1);
	printf("  GLTSD          : %x\n", ($page[0]>>1)&0x1);
	printf("  RLEC           : %x\n", ($page[0]>>0)&0x1);
	printf("  Q alg modifier : %x\n", ($page[1]>>4)&0xf);
	printf("  QErr           : %x\n", ($page[1]>>1)&0x3);
	printf("  VS             : %x\n", ($page[2]>>7)&0x1);
	printf("  rac            : %x\n", ($page[2]>>6)&0x1);
	printf("  ua_intlck_ctrl : %x\n", ($page[2]>>4)&0x3);
	printf("  swp            : %x\n", ($page[2]>>3)&0x1);
	printf("  ato            : %x\n", ($page[3]>>7)&0x1);
	printf("  tas            : %x\n", ($page[3]>>6)&0x1);
	printf("  autoload mode  : %x\n", ($page[3]>>0)&0x7);
	printf("  busy timeout   : %d\n", ($page[6]<<8)|($page[7]<<0));
	printf("  x selftest time: %d\n", ($page[8]<<8)|($page[9]<<0));
      }
      break;
    case 0x1aff:
      {
	printf("page 0x1a: Power Condition mode page (%s)\n", $ps);
	printf("  idle   : %x\n", ($page[1]>>1)&1);
	printf("  standby: %x\n", ($page[1]>>0)&1);
	printf("  idle condition timer   : %d*100ms\n",
	       ($page[2]<<24)|
	       ($page[3]<<16)|
	       ($page[4]<< 8)|
	       ($page[5]<< 0)|
	       0);
	printf("  standby condition timer: %d*100ms\n",
	       ($page[6]<<24)|
	       ($page[7]<<16)|
	       ($page[8]<< 8)|
	       ($page[9]<< 0)|
	       0);
      }
      break;
    case 0x0002:
      {
	printf("page 0x00-02: Copan Power Subpage (%s)\n", $ps);
	printf("  version        : %d\n", $page[0]);
	printf("  total_luns     : %d\n", $page[1]);
	printf("  max_active_luns: %d\n", $page[2]);
	printf("  cur_active_luns: %d\n", $page[3]);
      }
      break;
    case 0x00f0:
      {
	printf("page 0x00-F0: Copan Debug Configuration Subpage (%s)\n", $ps);
	printf("  version        : %d\n", $page[0]);
	printf("  time_io_secs   : %d\n", ($page[1]<<8)|($page[2]<<0));
      }
      break;
    default:
      {
        int $bytenum;
        printf("page 0x%.2x%s: (unimplemented) (%s)\n", $type, subpagestr, $ps);
        for ($bytenum=0; $bytenum < $len+headsize; $bytenum++) {
          if ($bytenum % 16 == 0) {
            printf("%.3x:", $bytenum);
          }
          if ($bytenum %  4 == 0) {
            printf(" ");
          }
          printf(" %.2x", datastart[$bytenum]);
          if ($bytenum % 16 == 15) {
            printf("\n");
          }
        }
        if ($bytenum % 16 != 0) {
          printf("\n");
        }
      }
    }
    printf("\n");
    free($page);
  }
}


void
PrintModeSenseSub6 (VECTOR dat)
{
   PrintModeSenseSub(dat, FALSE);
}


void
PrintModeSenseSub10(VECTOR dat)
{
   PrintModeSenseSub(dat, TRUE);
}


