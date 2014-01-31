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

#include "waveincontrol.h"

#include <qcombobox.h>
#include <q3groupbox.h>
#include <q3buttongroup.h>

#include "config/node_macro.h"

WaveInControl::WaveInControl( ObjectNode * config, QWidget* parent, const char* name, Qt::WFlags fl )
: WaveInControlBase(parent,name,fl), conf(config), m_channel(0)
{
}

WaveInControl::~WaveInControl()
{
	OBJECTVALUENODE_DECLARE_AND_GET(QString,device,conf);
	VALUENODE_SET_VALUE(device,ComboBoxCardSelection->currentText());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,channel,conf);
	VALUENODE_SET_VALUE(channel,m_channel);
}

void WaveInControl::inputDeviceChanged( int id )
{
	emit newSoundcardChosen( idxmap[id] );
}

void WaveInControl::inputChannelChanged( int c )
{
	m_channel = (Q_UINT8)c;
}

void WaveInControl::addSoundcard( QString str, int idx )
{
	ComboBoxCardSelection->insertItem(str);
	idxmap[ComboBoxCardSelection->count() - 1] = idx;
}

int WaveInControl::getChannel()
{
	return m_channel;
}

void WaveInControl::setPause( bool b )
{
	GroupBoxCardSelection->setEnabled(b);
}

void WaveInControl::fireConfiguration()
{
	OBJECTVALUENODE_DECLARE_AND_GET(QString,device,conf);
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,channel,conf);

	// Set defaults
	ComboBoxCardSelection->setCurrentItem(0);
	m_channel = 0;

	// Try to set configuration
	for(int i = 0; i < ComboBoxCardSelection->count(); i++)
	{
		if(VALUENODE_GET_VALUE(device) == ComboBoxCardSelection->text(i))
		{
			ComboBoxCardSelection->setCurrentItem(i);
			BG_channel->setButton(VALUENODE_GET_VALUE(channel));
			m_channel = VALUENODE_GET_VALUE(channel);
		}
	}

	// Update configuration
	emit newSoundcardChosen( idxmap[ComboBoxCardSelection->currentItem()] );
}
