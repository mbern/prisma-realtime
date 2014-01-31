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

#ifndef PI_DATA_QUEUE_H
#define PI_DATA_QUEUE_H

#include <quuid.h>

#include "pi_unknown.h"

// {2AEFDE30-FE44-4f52-9912-6BE64E9188E0}
#ifndef PIID_DATA_QUEUE
#define PIID_DATA_QUEUE QUuid( 0x2aefde30, 0xfe44, 0x4f52, 0x99, 0x12, 0x6b, 0xe6, 0x4e, 0x91, 0x88, 0xe0)
#endif

class PI_DataObj;

class PI_DataQueue : public PI_Unknown
{
public:
	virtual PI_DataObj * dequeue() = 0;
	virtual int bufferLength() = 0;
};

#endif //PI_DATA_QUEUE_H
