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

#ifndef FEATURE_DATA_H
#define FEATURE_DATA_H

#include <qobject.h>

#include "glgraphbase.h"
#include "graphdefs.h"
#include "doublebuffer.h"

class ObjectNode;

class FeatureData : public QObject
{
	Q_OBJECT

public:
	FeatureData( ObjectNode * config );
	~FeatureData();

	void init();
	void cleanup();
	void addData(double *pD, unsigned int length, double reftime);
	void update();
	void draw( float offset, t_curves_mask c_mask );

protected slots:
	void newTuning( int t ) { m_RefTuning = t; };

private:
	unsigned int c_nframes;
	uint m_VBO[NUM_SHORTTIME_FEATURES];
	int m_DataIndex;
	ObjectNode * m_Config;
	int m_RefTuning;
};

#endif //FEATURE_DATA_H
