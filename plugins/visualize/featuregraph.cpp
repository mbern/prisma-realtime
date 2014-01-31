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

#include "featuregraph.h"
#include "featuredata.h"

FeatureGraph::FeatureGraph( QWidget * w, int id, FeatureData * pSD )
: BaseGraph(w,id),m_Curves(m_StateControl),m_pFeatureData(pSD)
{
	GraphState state;
	state.isVisible = true;
	state.xLow = -3.4;
	state.xHigh = 0;
	state.xFracDigit = 1;
	state.yLow = -100;
	state.yHigh = 0;
	state.yFracDigit = 0;

	t_curves_mask cm;
	memset(&cm,0,sizeof(t_curves_mask));

	m_StateControl.clear();
	m_Curves.setSecondaryBuffer(cm);
	m_GraphState.setSecondaryBuffer(state);
	m_StateControl.set();
}

FeatureGraph::~FeatureGraph()
{
}

void FeatureGraph::setValues( const ShorttimeFeatureState & state )
{
	GraphState gstate = state.gstate;

	m_StateControl.clear();
	m_Curves.setSecondaryBuffer(state.curves);
	m_GraphState.setSecondaryBuffer(gstate);
	m_StateControl.set();
}

void FeatureGraph::setDataViewport()
{
	const GraphState & state = m_GraphState.getPrimaryBuffer();
	const QRect &rect = m_DataPos.getPrimaryBuffer();

	glViewport( rect.left(), rect.top(), rect.width(), rect.height() );
	glClear( GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( state.xLow, state.xHigh, state.yLow, state.yHigh, 1, -1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0, 0, 0, 0, 0, -1, 0, 1, 0 );
}

void FeatureGraph::drawData( double pos )
{
	m_pFeatureData->draw(pos,m_Curves.getPrimaryBuffer());
}
