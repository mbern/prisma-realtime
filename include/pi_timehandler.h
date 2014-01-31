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

#ifndef PI_TIME_HANDLER_H
#define PI_TIME_HANDLER_H

#include "pi_unknown.h"

// {732303D0-3C7E-408a-B115-A332646F7B89}
#ifndef PIID_TIME_PROVIDER
#define PIID_TIME_PROVIDER QUuid( 0x732303d0, 0x3c7e, 0x408a, 0xb1, 0x15, 0xa3, 0x32, 0x64, 0x6f, 0x7b, 0x89 )
#endif

// {B13128DA-FC79-4cd8-95B2-DCA4EA6089BB}
#ifndef PIID_TIME_CONSUMER
#define PIID_TIME_CONSUMER QUuid( 0xb13128da, 0xfc79, 0x4cd8, 0x95, 0xb2, 0xdc, 0xa4, 0xea, 0x60, 0x89, 0xbb )
#endif

class PI_TimeProvider : virtual public PI_Unknown
{
public:
	virtual double getTime() = 0;
protected:
	PI_TimeProvider() {}
	virtual ~PI_TimeProvider() {};
};

class PI_TimeConsumer : virtual public PI_Unknown
{
public:
	virtual void connectToProvider( PI_TimeProvider * provider ) = 0;
protected:
	PI_TimeConsumer() {}
	virtual ~PI_TimeConsumer() {};
};

#endif // PI_TIME_HANDLER_H
