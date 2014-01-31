/*
 * prisma-realtime analyzes and visualizes monophonic musical instrument tones
 * Copyright (C) 2014  Michael Bernhard
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PI_DATA_ROUTING_H
#define PI_DATA_ROUTING_H

#include <quuid.h>

#include "pi_unknown.h"

// {69CF0DF5-5AEB-4fe1-8681-2012AC6C457D}
#ifndef PIID_DATA_ROUTING
#define PIID_DATA_ROUTING QUuid( 0x69cf0df5, 0x5aeb, 0x4fe1, 0x86, 0x81, 0x20, 0x12, 0xac, 0x6c, 0x45, 0x7d )
#endif

class PI_DataRouting : public PI_Unknown
{

};

#endif //PI_DATA_ROUTING_H
