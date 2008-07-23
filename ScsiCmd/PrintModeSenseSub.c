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
    byte *$page;
    byte subpage = -1;
    byte $rawtype, $len;

    if (2 != myread($q, 2))
      return;

    $rawtype = $q[0];
    $ps = $rawtype >> 7 ? "savable" : "not savable";	/* top bit */
    $spf = ($rawtype >> 6) & 1;
    $type = $rawtype & 0x3f;	/* bottom six bits */
    if ($spf) {
      subpage = $q[1];
      if (2 != myread($q, 2))
        return;
      $len    = ($q[0] << 8) | ($q[1] << 0);
      $totlen -= $len+4;
    } else {
      $len     = $q[1];
      $totlen -= $len+2;
    }
    $page = malloc($len);
    if ($len != myread($page, $len)) {
      free($page);
      return;
    }

    switch ($type) {
    case 0x1d:
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
    case 0x1e:
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
    case 0x1f:
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
    case 0x30:
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
    default:
      {
        int $bytenum;
        printf("page 0x%.2x: (unimplemented) (%s)\n", $type, $ps);
        printf("%.3x:  %.2x %.2x", 0, $rawtype, $len);
        for ($bytenum=2; $bytenum-2 < $len; $bytenum++) {
          if ($bytenum % 16 == 0) {
            printf("%.3x:", $bytenum);
          }
          if ($bytenum %  4 == 0) {
            printf(" ");
          }
          printf(" %.2x", $page[$bytenum-2]);
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


