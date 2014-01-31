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

#ifndef PI_GUI_CONTAINER_H
#define PI_GUI_CONTAINER_H

#include <quuid.h>

#include "pi_unknown.h"

// {FC83396F-4B2A-4596-BBA4-B354D6BE9926}
#ifndef PIID_GUI_CONTAINER
#define PIID_GUI_CONTAINER QUuid( 0xfc83396f, 0x4b2a, 0x4596, 0xbb, 0xa4, 0xb3, 0x54, 0xd6, 0xbe, 0x99, 0x26)
#endif

class QWidget;
class QObject;
class QString;

class PI_GuiContainer : virtual public PI_Unknown
{
public:
	virtual void setParentWindow( QWidget * pParent, const QString & widget ) = 0;
	virtual void connectSlot( const QString & signal, const QObject * obj, const QString & slot ) = 0;
	virtual QObject * getObject( const QString & obj ) = 0;
	virtual QWidget * getWidget( const QString & widget ) = 0;
protected:
	PI_GuiContainer() {}
	virtual ~PI_GuiContainer() {};
};

#endif // PI_GUI_CONTAINER_H
