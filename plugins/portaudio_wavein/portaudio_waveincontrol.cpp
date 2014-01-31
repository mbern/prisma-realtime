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

#include "portaudio_waveincontrol.h"

#include <qcombobox.h>
#include <q3groupbox.h>

#include "config/node_macro.h"

PaWaveInControl::PaWaveInControl( ObjectNode * config, QWidget* parent, const char* name, Qt::WFlags fl )
: PaWaveInControlBase(parent,name,fl), conf(config)
{
}

PaWaveInControl::~PaWaveInControl()
{
	OBJECTVALUENODE_DECLARE_AND_GET(QString,device,conf);
	VALUENODE_SET_VALUE(device,ComboBoxCardSelection->currentText());
}

void PaWaveInControl::inputDeviceChanged( int id )
{
	emit newSoundcardChosen( idxmap[id] );
}

void PaWaveInControl::addSoundcard( QString str, int idx )
{
	ComboBoxCardSelection->insertItem(str);
	idxmap[ComboBoxCardSelection->count() - 1] = idx;
}

void PaWaveInControl::setPause( bool b )
{
	GroupBoxCardSelection->setEnabled(b);
}

void PaWaveInControl::fireConfiguration()
{
	OBJECTVALUENODE_DECLARE_AND_GET(QString,device,conf);
	for(int i = 0; i < ComboBoxCardSelection->count(); i++)
	{
		if(VALUENODE_GET_VALUE(device) == ComboBoxCardSelection->text(i))
		{
			ComboBoxCardSelection->setCurrentItem(i);
			emit newSoundcardChosen( idxmap[i] );
			return;
		}
	}
	emit newSoundcardChosen( idxmap[ComboBoxCardSelection->currentItem()] );
}
