

#ifdef ENUM
  CMD_Help,
#endif


#ifdef DEF
  {CMD_Help, "help", LineHelp, DIRECTION_NONE, NULL, "", "print list of commands"},
#endif


#ifdef LINE
#include "CmdHelp.h"

int
LineHelp(SCSI_HANDLE handle, COMMON_PARAMS common,
	 int argc, char**argv)
{
  common->stt.len = 0;  /* stop main from printing status */

  if (argc > optind) {
    help(common);
    return -1;
  }
  
  usage();

  return 0;
}
#endif


#ifdef COMMAND
#endif


