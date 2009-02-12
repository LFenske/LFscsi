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
#ifndef __PrintReadCapacitySub_h
#define __PrintReadCapacitySub_h

#include "common.h"

void PrintReadCapacitySub  (VECTOR dat, bool longdata);
void PrintReadCapacitySub10(VECTOR dat);
void PrintReadCapacitySub16(VECTOR dat);

#endif /* __PrintReadCapacitySub_h */
