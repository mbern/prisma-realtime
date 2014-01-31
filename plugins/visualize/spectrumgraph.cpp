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

#include "spectrumgraph.h"

#include <math.h>

#include <qapplication.h>

#include "spectrumdata.h"


SpectrumGraph::SpectrumGraph( QWidget * w, int id, SpectrumData * pSD )
: BaseGraph(w,id), m_State(m_StateControl), m_pSpectrumData(pSD)
{
}

SpectrumGraph::~SpectrumGraph()
{
	delete m_pGlFont;
}

void SpectrumGraph::setValues( const ViewState & state )
{
	m_StateControl.clear();

	m_State.setSecondaryBuffer(state);
	GraphState gstate;
	gstate.isVisible = state.isVisible;
	switch(state.eView)
	{
	case FA_Atlas:
		// x-axis = frequency
		gstate.xLow = state.fLow;
		gstate.xHigh = state.fHigh;
		gstate.xFracDigit = 0;
		// y-axis = amplitude
		gstate.yLow = state.aLow;
		gstate.yHigh = state.aHigh;
		gstate.yFracDigit = 0;
		break;
	case TA_Atlas:
		// x-axis = time
		gstate.xLow = state.tLow;
		gstate.xHigh = state.tHigh;
		gstate.xFracDigit = 1;
		// y-axis = amplitude
		gstate.yLow = state.aLow;
		gstate.yHigh = state.aHigh;
		gstate.yFracDigit = 0;
		break;
	case TF_Atlas:
	case TF_Spectrum:
		// x-axis = time
		gstate.xLow = state.tLow;
		gstate.xHigh = state.tHigh;
		gstate.xFracDigit = 1;
		// y-axis = frequency
		gstate.yLow = state.fLow;
		gstate.yHigh = state.fHigh;
		gstate.yFracDigit = 0;
		break;
	}
	m_GraphState.setSecondaryBuffer(gstate);

	m_StateControl.set();
}

void SpectrumGraph::setDataViewport()
{
	const ViewState &state = m_State.getPrimaryBuffer();
	const QRect &rect = m_DataPos.getPrimaryBuffer();

	//glViewport( 40, rect.bottom()+20, rect.width()-60, rect.height()-30 );
	glViewport( rect.left(), rect.top(), rect.width(), rect.height() );
	glClear( GL_DEPTH_BUFFER_BIT );
	switch( state.eView )
	{
	case FA_Atlas:
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( state.fLow, state.fHigh, state.aLow, state.aHigh, state.tHigh, -state.tLow );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( 0, 0, 0, 0, 0, -1, 0, 1, 0 );
		break;
	case TF_Atlas:
	case TF_Spectrum:
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( state.tLow, state.tHigh, state.fLow, state.fHigh, -state.aHigh, -state.aLow );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( 0, 0, 0, 0, -1, 0, 1, 0, 0 );
		break;
	case TA_Atlas:
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( -state.tLow, state.tHigh, state.aLow, state.aHigh, 20000 - state.fHigh, 20000 - state.fLow );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( 20000, 0, 0, 0, 0, 0, 0, 1, 0 );
		break;
	}
	glEnable( GL_DEPTH_TEST );

}

void SpectrumGraph::drawData( double pos )
{
	m_pSpectrumData->draw(m_State.getPrimaryBuffer(),pos);
}
