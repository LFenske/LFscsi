#define __PrintModeSenseSub_c

#include "common.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy, strcat */
#include <ctype.h>    /* for isprint */


static VECTOR myread_data;


static void
myread_init(VECTOR dat)
{
  myread_data = dat;
}


static int
myread(unsigned char *pC, int len)
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
PrintModeSenseSub(VECTOR dat)
{
  unsigned char $density, $type;
  char *$ps;
  char qc[16];
  unsigned char $totlen, $medtype, $WPcache, $BDL;

  if (dat.dat == NULL)
    return;

  myread_init(dat);

  if (4 != myread(qc, 4))
    return;

  $totlen  = qc[0];
  $medtype = qc[1];
  $WPcache = qc[2];
  $BDL     = qc[3];
  $totlen -= 3;
  if ($BDL) {
    int $numblocks, $blocklen;
    unsigned char *$q;
    $q = malloc($BDL);
    if ($BDL != myread($q, $BDL)) {
      free($q);
      return;
    }
    $numblocks = ($q[0] << 24) | ($q[1] << 16) | ($q[2] <<  8) | ($q[3] <<  0);
    $blocklen  = ($q[4] << 24) | ($q[5] << 16) | ($q[6] <<  8) | ($q[7] <<  0);
    $totlen -= $BDL;
    $density = $numblocks >> 24;
    $numblocks &= 0xffffff;
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
  while ($totlen) {
    unsigned char $q[2];
    unsigned char *$page;
    unsigned char $rawtype, $len;

    if (2 != myread($q, 2))
      return;

    $rawtype = $q[0];
    $len     = $q[1];
    $page = malloc($len);
    if ($len != myread($page, $len)) {
      free($page);
      return;
    }
    $totlen -= $len+2;
    $ps = $rawtype >> 7 ? "savable" : "not savable";	/* top bit */
    $type = $rawtype & 0x3f;	/* bottom six bits */

    switch ($type) {
    case 0x1d:
      {
        short $MTad, $MTnm, $STad, $STnm, $IEad, $IEnm, $DTad, $DTnm;
        unsigned char *pp = $page;
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
        unsigned char $rotate, $transport;
        printf("page 0x1e: Transport Geometry (%s)\n", $ps);
        for ($bytenum=0; $bytenum < $len; $bytenum += 2) {
          $rotate    = $page[$bytenum + 0];
          $transport = $page[$bytenum + 1];
          printf("  Transport %d is %s\n", $transport, $rotate&1 ? "rotatable" : "non-rotatable");
        }
      }
    case 0x1f:
      {
        printf("page 0x1f: Device Capabilities (%s)\n", $ps);
        unsigned char $stor, $frMT, $frST, $frIE, $frDT, $exMT, $exST, $exIE, $exDT;
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


