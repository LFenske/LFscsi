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


