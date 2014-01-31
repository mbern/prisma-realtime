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

#include "marker.h"
#include "basegraph.h"

Marker::Marker( BaseGraph * dest, GLint factor, GLushort pattern, GLfloat width)
: m_Value(m_MarkerBufferControl), m_Visible(m_MarkerBufferControl), m_Type(m_MarkerBufferControl), m_GrayLevel(m_MarkerBufferControl),
  m_Destination(dest), m_LinePatternFactor(factor), m_LinePattern(pattern), m_LineWidth(width)
{
	m_Destination->addMarker( this );
	m_Visible.setSecondaryBuffer(true);
	m_Type.setSecondaryBuffer(Marker::M_V);
	m_MarkerBufferControl.set();
}

void Marker::init()
{
	m_ListBase = glGenLists( 1 );
}

void Marker::setValue( double val )
{
	m_MarkerBufferControl.clear();
	m_Value.setSecondaryBuffer(val);
	m_MarkerBufferControl.set();
}

void Marker::setVisible( bool b )
{
	m_MarkerBufferControl.clear();
	m_Visible.setSecondaryBuffer(b);
	m_MarkerBufferControl.set();
}

void Marker::setDirection( int d )
{
	if( d == Marker::M_H || d == Marker::M_V )
	{
		m_MarkerBufferControl.clear();
		m_Type.setSecondaryBuffer((Marker::MarkerDirection)d);
		m_MarkerBufferControl.set();
	}
}

void Marker::setGrayLevel( unsigned char g )
{
	m_MarkerBufferControl.clear();
	m_GrayLevel.setSecondaryBuffer(g);
	m_MarkerBufferControl.set();
}

void Marker::update(bool force)
{
	GLubyte g;

	bool markerflag = m_MarkerBufferControl.update();

	if( markerflag || force )
	{
		double value = m_Value.getPrimaryBuffer();
		const GraphState & state = m_Destination->getGraphState();
		const QRect & datarect = m_Destination->getDataPos();
		const MarkerDirection & type = m_Type.getPrimaryBuffer();

		// Remove Marker if out of range
		if( (type == Marker::M_V && (value <= state.xLow || value >= state.xHigh)) ||
			(type == Marker::M_H && (value <= state.yLow || value >= state.yHigh)))
		{
			glNewList( m_ListBase, GL_COMPILE );
			glEndList();
			return;
		}

		// Scale correctly
		double xRatio = datarect.width()/(double)(state.xHigh-state.xLow);
		double yRatio = datarect.height()/(double)(state.yHigh-state.yLow);
		double xOffset = -state.xLow;
		double yOffset = -state.yLow;

		glNewList( m_ListBase, GL_COMPILE );
		{
			glPushAttrib( GL_LINE_BIT );
			{
				if( m_LinePatternFactor > 0 )
				{
					glEnable( GL_LINE_STIPPLE );
					glLineStipple( m_LinePatternFactor, m_LinePattern );
				}
				g = m_GrayLevel.getPrimaryBuffer();
				glColor3ub( g, g, g );
				glLineWidth( m_LineWidth );

				glBegin( GL_LINES );
				{
					switch(type)
					{
					case Marker::M_V:
						glVertex2f( (value+xOffset)*xRatio, (state.yLow+yOffset)*yRatio );
						glVertex2f( (value+xOffset)*xRatio, (state.yHigh+yOffset)*yRatio );
						break;
					case Marker::M_H:
						glVertex2f( (state.xLow+xOffset)*xRatio, (value+yOffset)*yRatio );
						glVertex2f( (state.xHigh+xOffset)*xRatio, (value+yOffset)*yRatio );
						break;
					}
				}
				glEnd();
			}
			glPopAttrib();
		}
		glEndList();
	}
}

void Marker::draw()
{
	if( m_Visible.getPrimaryBuffer() )
		glCallList( m_ListBase );
}
