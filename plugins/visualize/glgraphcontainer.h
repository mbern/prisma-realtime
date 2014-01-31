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

#ifndef GRAPH_WIDGET_H
#define GRAPH_WIDGET_H

#include <q3valuelist.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QTimerEvent>

#include "glgraphbase.h"
#include "doublebuffer.h"
#include "safebuffer.h"
#include "spectrumdata.h"
#include "featuredata.h"
#include "basegraph.h"

class Visualize;
class StreamContainer;
struct ViewState;

class ObjectNode;

class GlGraphContainer : public GLGraphBase
{
	Q_OBJECT
public:

	GlGraphContainer( ObjectNode * config, Visualize * root, const QGLFormat  & format, QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0  );

	virtual ~GlGraphContainer();

	virtual void addGraphObject( BaseGraph * pBG )
	{
		m_Graphs.append( pBG );
		m_Layout->addItem( pBG );
	}

	virtual void addDataObject( SpectrumData * pSD )
	{
		m_pSpectrumData = pSD;
	}

	virtual void addDataObject( FeatureData * pSD )
	{
		m_pFeatureData = pSD;
	}

public slots:
	void setPause( bool b )
	{
		if(b)
			this->blockSignals(true);
		else
			this->blockSignals(false);
		m_ConfigControl.clear();
		m_isPause.setSecondaryBuffer(b);
		m_ConfigControl.set();
	}
	void setBackgroundGray( unsigned char g )
	{
		m_ConfigControl.clear();
		m_backgroundcolor.setSecondaryBuffer(g);
		m_ConfigControl.set();
	}
	void setPointSize( int s )
	{
		m_ConfigControl.clear();
		m_pointsize.setSecondaryBuffer(s);
		m_ConfigControl.set();
	}

signals:
	void newBaseFreq( double );
	void newMeterValue( double, double );


protected:
	void myPaintGL( double offset );
	void myInitializeGL();
	void myResizeGL( int w, int h );
	void myUpdateStartGL();
	void myCleanupGL();

	double getTime();
	bool getRunState() {return m_isPause.getSecondaryBuffer();}

	int myUpdateDataGL();

	void timerEvent( QTimerEvent * e );

private:
	static const int c_idisplaybuffersize;

	DoubleBufferControl m_ConfigControl;
	::DoubleBuffer<bool> m_isPause;
	::DoubleBuffer<unsigned char> m_backgroundcolor;
	::DoubleBuffer<int> m_pointsize;
	bool m_isConfigUpdated;

	Visualize * m_root;

	SafeBuffer<double> m_f0List;
	SafeBuffer<double> m_MaxList;
	SafeBuffer<double> m_RmsList;

	double m_OffsetBak;

	StreamContainer * m_pCompound;

	Q3PtrList<BaseGraph> m_Graphs;
	SpectrumData * m_pSpectrumData;
	FeatureData * m_pFeatureData;

	ObjectNode * conf;

	QLayout * m_Layout;
};

#endif //GRAPH_WIDGET_H
