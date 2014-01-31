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

#include "basegraph.h"

#include <qapplication.h>
//Added by qt3to4:
#include <QEvent>

#include <math.h>

#include "glfont.h"

const unsigned char BaseGraph::c_ubgridalpha = 150;

BaseGraph::BaseGraph( QWidget * parent, int id )
: m_GraphState(m_StateControl), m_AxisColor(m_StateControl), m_GraphPos(m_PosControl), m_DataPos(m_PosControl),
  m_Parent(parent), m_id(id)
{
	m_Markers.setAutoDelete( true );
	m_pGlFont = GLFont::createGLFont( "graphfont.fnt", 500 );
	CColor col = {200,200,200,255};
	m_AxisColor.setSecondaryBuffer(col);
	m_StateControl.set();
}

void BaseGraph::init()
{
	m_GridListBase = glGenLists( 1 );

	Marker * marker = NULL;
	Q3PtrListIterator< Marker > it( m_Markers );
	while( ( marker = it.current() ) != NULL )
	{
		++it;
		marker->init();
	}
	m_pGlFont->init();
}

void BaseGraph::cleanup()
{
}

void BaseGraph::validate()
{
	bool oldvisible = m_GraphState.getPrimaryBuffer().isVisible;

	bool stateflag = m_StateControl.update();
	bool graphposflag = m_PosControl.update();

	bool newvisible = m_GraphState.getPrimaryBuffer().isVisible;

	if( newvisible )
	{
		if( graphposflag || stateflag )
		{
			updateDiv();
			updateGrid();
			updateText();
		}

		updateMarker( graphposflag || stateflag );
	}

	if( oldvisible != newvisible )
		qApp->postEvent( m_Parent, new QEvent( QEvent::LayoutHint) );
}

void BaseGraph::draw( double pos )
{
	const QRect & datarect = m_DataPos.getPrimaryBuffer();
	const QRect & graphrect = m_GraphPos.getPrimaryBuffer();

	if( graphrect.isEmpty() || !graphrect.isValid() )
		return;

	setDataViewport();

	drawData(pos);

	setGraphViewport();

	glTranslated( datarect.left(), datarect.top()-graphrect.top(), 0 );
	drawMarker();

	glTranslated( 0, 0, -0.2 );
	drawGrid();
	drawText();
}

void BaseGraph::addMarker( Marker * m )
{
	m_Markers.append( m );
}

// Methoden von QLayoutItem
QSize BaseGraph::sizeHint () const
{
	if( !isEmpty() )
		return QSize(600,200);
	else
		return QSize(0,0);
}

QSize BaseGraph::minimumSize () const
{
	if( !isEmpty() )
		return QSize(400,150);
	else
		return QSize(0,0);
}

QSize BaseGraph::maximumSize () const
{
	if( !isEmpty() )
		return QSize(32767,32767);
	else
		return QSize(0,0);
}

Qt::Orientations BaseGraph::expandingDirections () const
{
	return Qt::Horizontal | Qt::Vertical;
}

void BaseGraph::setGeometry ( const QRect & r )
{
	m_PosControl.clear();

	m_GraphPos.setSecondaryBuffer(r);
	m_DataPos.setSecondaryBuffer(QRect( r.left()+40, r.top()+20, r.width()-60, r.height()-30 ));

	m_PosControl.set();
}

QRect BaseGraph::geometry () const
{
	return m_GraphPos.getSecondaryBuffer();
}

bool BaseGraph::isEmpty () const
{
	return !m_GraphState.getSecondaryBuffer().isVisible;
}

void BaseGraph::setGrayLevel( unsigned char g )
{
	GLubyte gray = g + (GLubyte)128;
	m_StateControl.clear();
	CColor col;
	col.r = gray;
	col.g = gray;
	col.b = gray;
	m_AxisColor.setSecondaryBuffer(col);
	m_StateControl.set();

	for( Q3PtrListIterator< Marker > it( m_Markers ); it.current() != NULL; ++it )
	{
		it.current()->setGrayLevel(gray);
	}

}

void BaseGraph::updateDiv()
{
	const GraphState & state = m_GraphState.getPrimaryBuffer();
	const QRect & datarect = m_DataPos.getPrimaryBuffer();
	Metrics & m = m_Metrics;

	// Save graph size
	m.width = datarect.width();
	m.height = datarect.height();

	// Scale correctly
	m.xRatio = datarect.width()/(double)(state.xHigh-state.xLow);
	m.yRatio = datarect.height()/(double)(state.yHigh-state.yLow);
	m.xOffset = -state.xLow;
	m.yOffset = -state.yLow;

	// Estimate text size
	int strlenmax, strlenmin, strlenchoose, charheight;
	if( state.xFracDigit > 0)
	{
		strlenmax = m_pGlFont->stringLength(QString::number(state.xHigh,'f',state.xFracDigit));
		strlenmin = m_pGlFont->stringLength(QString::number(state.xLow,'f',state.xFracDigit));
	}
	else
	{
		strlenmax = m_pGlFont->stringLength(QString::number((int)state.xHigh));
		strlenmin = m_pGlFont->stringLength(QString::number((int)state.xLow));
	}
	if(strlenmax > strlenmin)
		strlenchoose = strlenmax;
	else
		strlenchoose = strlenmin;

	charheight = m_pGlFont->charHeight('0');

	// Calculate the grid
	m.xDiv = calculateDiv( state.xHigh, state.xLow, state.xFracDigit, datarect.width() / (1.8*strlenchoose) );
	m.yDiv = calculateDiv( state.yHigh, state.yLow, state.yFracDigit, datarect.height() / (2.5*charheight) );
}

void BaseGraph::updateGrid()
{
	float v;
	int i;

	const GraphState & state = m_GraphState.getPrimaryBuffer();
	const Metrics & m = m_Metrics;

	glNewList( m_GridListBase, GL_COMPILE );
	{
		// Gitter
		glLineWidth(1);
		glLineStipple( 1, 0xAAAA );
		glEnable( GL_LINE_STIPPLE );
		glBegin( GL_LINES );
		{
			for( i = (int)ceil(state.xLow/m.xDiv); i <= (int)floor(state.xHigh/m.xDiv); i++ )
			{
				v = i * m.xDiv;
				glVertex2f( (v+m.xOffset)*m.xRatio, (state.yLow+m.yOffset)*m.yRatio );
				glVertex2f( (v+m.xOffset)*m.xRatio, (state.yHigh+m.yOffset)*m.yRatio );
			}
			for( i = (int)ceil(state.yLow/m.yDiv); i <= (int)floor(state.yHigh/m.yDiv); i++ )
			{
				v = i * m.yDiv;
				glVertex2f( (state.xLow+m.xOffset)*m.xRatio, (v+m.yOffset)*m.yRatio );
				glVertex2f( (state.xHigh+m.xOffset)*m.xRatio, (v+m.yOffset)*m.yRatio );
			}
		}
		glEnd();

		// Rahmen
		glLineWidth(2);
		glDisable( GL_LINE_STIPPLE );
		glBegin( GL_LINE_LOOP );
		{
			glVertex2d( 0, 0 );
			glVertex2d( 0, m.height );
			glVertex2d( m.width, m.height );
			glVertex2d( m.width, 0 );
		}
		glEnd();

	}
	glEndList();
}

void BaseGraph::updateText()
{
	int strwidth;
	int i;
	float v;

	const GraphState & state = m_GraphState.getPrimaryBuffer();
	const Metrics & m = m_Metrics;

	QString textstr;

	m_pGlFont->reset();

	m_pGlFont->addTextTop( QString::number(m_id), QPoint( m.width+5, m.height+2 ) );

	for( i = (int)ceil(state.xLow/m.xDiv); i <= (int)floor(state.xHigh/m.xDiv); i++ )
	{
		v = i * m.xDiv;
		if(state.xFracDigit > 0)
			textstr.setNum( v, 'f', state.xFracDigit );
		else
			textstr.setNum( (int)v );
		strwidth = m_pGlFont->stringLength( textstr );
		m_pGlFont->addTextTop( textstr, QPoint( (int)((v+m.xOffset)*m.xRatio-strwidth/2), -3 ) );
	}
	for( i = (int)ceil(state.yLow/m.yDiv); i <= (int)floor(state.yHigh/m.yDiv); i++ )
	{
		v = i * m.yDiv;
		if(state.yFracDigit > 0)
			textstr.setNum( v, 'f', state.yFracDigit );
		else
			textstr.setNum( (int)v );
		strwidth = m_pGlFont->stringLength( textstr );
		m_pGlFont->addTextBase( textstr, QPoint( -strwidth-4, (int)((v+m.yOffset)*m.yRatio-3) ) );
	}
}

void BaseGraph::updateMarker(bool force)
{
	for( Q3PtrListIterator< Marker > it( m_Markers ); it.current() != NULL; ++it )
	{
		it.current()->update(force);
	}
}

void BaseGraph::setGraphViewport()
{
	const QRect &rect = m_GraphPos.getPrimaryBuffer();
//	glViewport( 0, 0, m_WindowSize[m_WindowSizeFlag.iDraw()].width(), m_WindowSize[m_WindowSizeFlag.iDraw()].height() );
	glViewport( rect.left(), rect.top(), rect.width(), rect.height() );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
//	glOrtho( 0, m_WindowSize[m_WindowSizeFlag.iDraw()].width(), 0, m_WindowSize[m_WindowSizeFlag.iDraw()].height(), 1, -1 );
	glOrtho( 0, rect.width(), 0, rect.height(), 1, -1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glClear( GL_DEPTH_BUFFER_BIT );
	//glDisable( GL_DEPTH_TEST );
}

void BaseGraph::drawGrid()
{
	CColor col = m_AxisColor.getPrimaryBuffer();
	col.a = c_ubgridalpha;
	glColor4ubv((GLubyte*)&col);
	glCallList( m_GridListBase );
}

void BaseGraph::drawText()
{
	CColor col = m_AxisColor.getPrimaryBuffer();
	glColor3ubv((GLubyte*)&col);

	m_pGlFont->drawText();
}

void BaseGraph::drawMarker()
{
	Marker *marker = NULL;
	Q3PtrListIterator< Marker > it( m_Markers );
	while( ( marker = it.current() ) != NULL )
	{
		++it;
		marker->draw();
	}
}

double BaseGraph::calculateDiv( double high, double low, int fracdigit, int maxlines )
{
	double retval;
	double roundfac = (double)pow((double)10,fracdigit);
	double divraw = (high - low)/(double)maxlines;
	double divfloor = pow(10.0,floor(log10(divraw)));
	double divnorm = divraw/divfloor;

	if( divnorm <= 1 )
		retval = divfloor;
	else if( divnorm <= 2 )
		retval = 2*divfloor;
	else if( divnorm <= 5 )
		retval = 5*divfloor;
	else
		retval = 10*divfloor;

	retval = floor(retval * roundfac + 0.5) / roundfac;

	if( retval == 0 )
		retval = 1;

	return retval;
}
