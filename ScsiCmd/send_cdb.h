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
#ifndef __send_cdb_h
#define __send_cdb_h

#include "common.h"
#include "ScsiTransport.h"

#define NOSIZE  (-1)

#ifdef __send_cdb_c
#define GLOBAL
#else
#define GLOBAL extern
#endif


#ifdef __cplusplus
namespace LFscsi {
#endif

void send_cdb(SCSI_HANDLE device,
              COMMON_PARAMS common,
	      DIRECTION dir,
	      VECTOR cdb,
	      VECTOR dat,
	      float default_timeout);

#ifdef __cplusplus
}
#endif


#endif /* __send_cdb_h */
