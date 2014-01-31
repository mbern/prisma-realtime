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

#ifndef SPECTRUM_DATA_H
#define SPECTRUM_DATA_H

#include <qobject.h>

#include "glgraphbase.h"
#include "graphdefs.h"
#include "doublebuffer.h"

class ObjectNode;

class SpectrumData : public QObject
{
	Q_OBJECT
public:
	SpectrumData( ObjectNode * config );
	~SpectrumData();

	void init();
	void cleanup();
	void addData(double *A, double *F, unsigned int length, double f0, double reftime);
	void update();
	void draw( const struct ViewState & vs, float offset );

public slots:
	void setDeltaC( int dc )
	{
		m_ConfigControl.clear();
		m_ddeltac.setSecondaryBuffer(dc);
		m_ConfigControl.set();
	}

private:
	static const unsigned char c_ubcolormap[];
	static const unsigned char c_ubspectrummap[];
	static const GLint c_iFAfogMode;
	static const float c_fFAfogDensity;

	unsigned int c_nbufsize;
	unsigned int c_nframes;

	DoubleBufferControl m_ConfigControl;
	::DoubleBuffer<double> m_ddeltac;

	int *m_BufferSizes;
	int m_curVB;
	uint m_VBODataBuffer;
	uint m_VBOColor1Buffer;
	uint m_VBOColor2Buffer;

	ObjectNode * m_Config;
};

#endif //SPECTRUM_DATA_H
