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

#ifndef PI_CONFIGURATION_H
#define PI_CONFIGURATION_H

#include <quuid.h>

#include "pi_unknown.h"

// {619CCFF4-0E97-4b0e-8592-64078DBD8961}
#ifndef PIID_CONFIGURATION
#define PIID_CONFIGURATION QUuid(0x619ccff4, 0xe97, 0x4b0e, 0x85, 0x92, 0x64, 0x7, 0x8d, 0xbd, 0x89, 0x61)
#endif

class PluginNode;

class PI_Configuration : virtual public PI_Unknown
{
public:
	virtual bool configure(PluginNode * config) = 0;
	virtual bool stopRunning() = 0;
	virtual bool startRunning() = 0;
protected:
	PI_Configuration() {}
	virtual ~PI_Configuration() {};
};

#endif // PI_CONFIGURATION_H
