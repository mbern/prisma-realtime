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

#ifndef PI_DATA_OBJ_H
#define PI_DATA_OBJ_H

#include <quuid.h>
#include <qstring.h>

//#include "pi_unknown.h"

// {8D29EC8B-2165-4428-8AFD-3C7593B137D7}
#ifndef PIID_DATA_OBJ
#define PIID_DATA_OBJ QUuid( 0x8d29ec8b, 0x2165, 0x4428, 0x8a, 0xfd, 0x3c, 0x75, 0x93, 0xb1, 0x37, 0xd7 )
#endif


class PI_DataObj //: public PI_Unknown
{
public:
	virtual ~PI_DataObj() {};
	virtual Q_UINT32 type() = 0;
	virtual Q_UINT16 elementSize() = 0;
	virtual Q_UINT32 length() = 0;
	virtual Q_UINT32 allocatedLength() = 0;
	virtual bool isInteger() = 0;
	virtual QString name() = 0;
	virtual QString typeName() = 0;

	virtual void setType( Q_UINT32 type ) = 0;
	virtual void setLength( Q_UINT32 length ) = 0;

	virtual void * dataPtr() = 0;

	virtual PI_DataObj * createNewObj() = 0;
protected:
	PI_DataObj() {}
};

#endif // PI_DATA_OBJ_H
