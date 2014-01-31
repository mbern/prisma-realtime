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

#ifndef WAVE_IN_CONTROL_H
#define WAVE_IN_CONTROL_H

#include "portaudio_waveincontrolbase.h"

#include <qmap.h>

class ObjectNode;

class PaWaveInControl : public PaWaveInControlBase
{
	Q_OBJECT
public:
	PaWaveInControl( ObjectNode * config, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~PaWaveInControl();

	void addSoundcard( QString str, int idx );

public slots:
	void setPause( bool b );
	void fireConfiguration();

signals:
	void newSoundcardChosen( int );

protected slots:
	virtual void inputDeviceChanged( int );

private:
	ObjectNode * conf;
	QMap<int, int> idxmap;
};

#endif // WAVE_IN_CONTROL_H
