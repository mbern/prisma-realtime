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

#ifndef PI_DATA_BUFFER_H
#define PI_DATA_BUFFER_H

#include <quuid.h>
#include <qstringlist.h>

#include "pi_unknown.h"

// {B2852A92-6206-402a-8072-7165A52FD585}
#ifndef PIID_DATA_BUFFER_PROVIDER
#define PIID_DATA_BUFFER_PROVIDER QUuid( 0xb2852a92, 0x6206, 0x402a, 0x80, 0x72, 0x71, 0x65, 0xa5, 0x2f, 0xd5, 0x85 )
#endif

// {400E8B66-3855-457d-A83E-73BBF7168A60}
#ifndef PIID_DATA_BUFFER_CONSUMER
#define PIID_DATA_BUFFER_CONSUMER QUuid( 0x400e8b66, 0x3855, 0x457d, 0xa8, 0x3e, 0x73, 0xbb, 0xf7, 0x16, 0x8a, 0x60 )
#endif

class DataReader;

class PI_DataBufferProvider : virtual public PI_Unknown
{
public:
	virtual QStringList getBufferList() = 0;
	virtual DataReader * getDataReader( QString str ) = 0;
protected:
	PI_DataBufferProvider() {}
	virtual ~PI_DataBufferProvider() {};
};

class PI_DataBufferConsumer : virtual public PI_Unknown
{
public:
	virtual void connectToProvider( PI_DataBufferProvider * provider ) = 0;
protected:
	PI_DataBufferConsumer() {}
	virtual ~PI_DataBufferConsumer() {};
};

#endif // PI_DATA_BUFFER_H
