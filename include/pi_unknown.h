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

#ifndef PI_UNKNOWN_H
#define PI_UNKNOWN_H


#include <quuid.h>

// {4B441B40-89E4-493e-89E1-C3CB56843F07}
#ifndef PIID_UNKNOWN
#define PIID_UNKNOWN QUuid( 0x4b441b40, 0x89e4, 0x493e, 0x89, 0xe1, 0xc3, 0xcb, 0x56, 0x84, 0x3f, 0x7 )
#endif

class QString;

class PI_Unknown
{
public:
	virtual QString name() = 0;
	virtual int queryInterface( const QUuid piid, void ** ppInterface ) = 0;
	virtual int release() = 0;
	virtual int addRef() = 0;
protected:
	PI_Unknown() {}
	virtual ~PI_Unknown() {};
};

#endif // PI_UNKNOWN_H
