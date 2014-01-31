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

#ifndef SINGLE_GRAPH_H
#define SINGLE_GRAPH_H

#include <qsize.h>
#include <q3semaphore.h>
#include <qmutex.h>

#include "basegraph.h"
#include "glgraphbase.h"
#include "glfont.h"
#include "doublebuffer.h"
#include "graphdefs.h"
#include "marker.h"

class SpectrumData;

class SpectrumGraph : public BaseGraph
{
	Q_OBJECT
public:
	SpectrumGraph( QWidget * w, int id, SpectrumData * pSD );
	~SpectrumGraph();

public slots:
	void setValues( const ViewState & state );

protected:
	void setDataViewport();
	void drawData( double pos );

private:
	DoubleBuffer<ViewState> m_State;

	SpectrumData * m_pSpectrumData;
};

#endif // SINGLE_GRAPH_H
