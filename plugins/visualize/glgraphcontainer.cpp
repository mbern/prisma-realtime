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

#include "glgraphcontainer.h"

#include <math.h>

#include "visualize.h"
#include "spectrumgraph.h"
#include "streamcontainer.h"
#include "databuffer.h"
#include "graphdefs.h"

#include "config/node_macro.h"

#ifdef Q_OS_WIN
#include <GL/wglew.h>
#endif //Q_OS_WIN

#ifdef Q_WS_X11
#include <GL/glxew.h>
#endif //Q_WS_X11

//Added by qt3to4:
#include <QTimerEvent>
#include <Q3VBoxLayout>

const int GlGraphContainer::c_idisplaybuffersize = 100;

GlGraphContainer::GlGraphContainer( ObjectNode * config, Visualize * root, const QGLFormat  & format, QWidget * parent, const char * name, Qt::WFlags f  )
: GLGraphBase( format, parent,name,0,f),
  m_isPause(m_ConfigControl), m_backgroundcolor(m_ConfigControl), m_pointsize(m_ConfigControl),
  m_isConfigUpdated(false), m_root(root),
  m_f0List(c_idisplaybuffersize),m_MaxList(c_idisplaybuffersize),m_RmsList(c_idisplaybuffersize),conf(config)
{
	(void) config; //surpress warning

	m_Layout = new Q3VBoxLayout(this);

	m_pCompound = NULL;

	m_OffsetBak = 0;

	m_ConfigControl.clear();
	m_isPause.setSecondaryBuffer(false);
	m_backgroundcolor.setSecondaryBuffer(100);
	m_ConfigControl.set();

	startTimer( 333 );
}

GlGraphContainer::~GlGraphContainer()
{
	stop();
}

void GlGraphContainer::timerEvent( QTimerEvent * e )
{
	(void) e; // surpress warning

	int n;
	double value;
	double f0mean = 0;
	double maxmean = 0;
	double rmsmean = 0;

	n = 0;
	while(m_f0List.pop_front(&value))
	{
		n++;
		f0mean += value;
	}
	f0mean /= n;

	n = 0;
	while(m_MaxList.pop_front(&value))
	{
		n++;
		maxmean += value;
	}
	maxmean /= n;

	n = 0;
	while(m_RmsList.pop_front(&value))
	{
		n++;
		rmsmean += value;
	}
	rmsmean /= n;

	emit newBaseFreq( f0mean );
	emit newMeterValue( maxmean, rmsmean );
}

void GlGraphContainer::myUpdateStartGL()
{
	// Update configuration
	m_isConfigUpdated = m_ConfigControl.update();
	m_pSpectrumData->update();
	m_pFeatureData->update();
}

int GlGraphContainer::myUpdateDataGL()
{
	StreamContainer * tempobj ;


	// Try to get the next data object and relase the old one in this case
	tempobj = m_pCompound;
	m_pCompound = m_root->m_pCompoundReader->getNextObj();
	if(m_pCompound == NULL)
	{
		m_pCompound = tempobj;
		return FALSE;
	}
	else
	{
		if(tempobj != NULL)
			delete tempobj;
	}
	//Extract the frames
	PI_DataObj * aFrame = m_pCompound->getItem(PC_IDX_ANALYSIS_A);
	PI_DataObj * fFrame = m_pCompound->getItem(PC_IDX_ANALYSIS_F);
	PI_DataObj * sfFrame = m_pCompound->getItem(PC_IDX_ANALYSIS_SF);
	//Proceed only if we are not paused
	if( ! m_isPause.getPrimaryBuffer() )
	{
		unsigned int len = aFrame->length();
		double * pF = (double*)fFrame->dataPtr();
		double * pA = (double*)aFrame->dataPtr();
		double * pSF = (double*)sfFrame->dataPtr();

		m_pSpectrumData->addData( pA, pF, len, *pSF, m_pCompound->getRefTimeStart() );
		m_pFeatureData->addData( pSF, 4, m_pCompound->getRefTimeStart() );

		m_f0List.push_back(pSF[PV_IDX_ANA_SF_FUND_FREQ]);
		m_MaxList.push_back(pSF[PV_IDX_ANA_SF_MAX_AMP]);
		m_RmsList.push_back(pSF[PV_IDX_ANA_SF_RMS_AMP]);
	}

	return TRUE;
}


double GlGraphContainer::getTime()
{
	return m_root->m_pTimeProvider->getTime();
}


void GlGraphContainer::myInitializeGL()
{
	glEnable( GL_POINT_SMOOTH );
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );

//	glEnable( GL_POLYGON_SMOOTH );
//	glHint( GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE );

	glEnable(GL_DEPTH_TEST);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE );

#ifdef Q_OS_WIN
	wglSwapIntervalEXT( 1 );
#endif //Q_OS_WIN

#ifdef Q_WS_X11
	glXSwapIntervalSGI( 1 );
#endif //Q_WS_X11

	m_pSpectrumData->init();
	m_pFeatureData->init();

	for( BaseGraph * pgraph = m_Graphs.first(); pgraph; pgraph = m_Graphs.next() )
	{
		pgraph->init();
	}

	// Syncronize to first Databuffer
	while(m_pCompound == NULL)
	{
		m_pCompound = m_root->m_pCompoundReader->getNewestObj();
	}
}

void GlGraphContainer::myCleanupGL()
{
	m_pSpectrumData->cleanup();
	m_pFeatureData->cleanup();
	for( BaseGraph * pgraph = m_Graphs.first(); pgraph; pgraph = m_Graphs.next() )
	{
		pgraph->cleanup();
	}
}


void GlGraphContainer::myResizeGL( int w, int h )
{
	(void) w; //surpress warning
	(void) h; //surpress warning
}

void  GlGraphContainer::myPaintGL( double offset )
{
	if(m_isConfigUpdated)
	{
		// Set background color
		float col = m_backgroundcolor.getPrimaryBuffer()/255.0;
		GLfloat colorf[4];
		colorf[0] = col;
		colorf[1] = col;
		colorf[2] = col;
		colorf[3] = 1;

		glFogfv (GL_FOG_COLOR, colorf );
		glClearColor( colorf[0], colorf[1], colorf[2], colorf[3] );

		// Set point size
		glPointSize(m_pointsize.getPrimaryBuffer());
	}

	if( ! m_isPause.getPrimaryBuffer() )
		m_OffsetBak = offset;

	if( m_WindowSize.isEmpty() )
		return;

	glViewport( 0,0,m_WindowSize.width(),m_WindowSize.height());

	glClear( GL_COLOR_BUFFER_BIT );

	for( BaseGraph * pgraph = m_Graphs.first(); pgraph; pgraph = m_Graphs.next() )
	{
		pgraph->validate();
		pgraph->draw(m_OffsetBak);
	}
}
