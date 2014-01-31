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

#ifndef BASE_GRAPH_H
#define BASE_GRAPH_H

#include <qlayout.h>
#include <qrect.h>
#include <q3ptrlist.h>

#include "doublebuffer.h"
#include "glgraphbase.h"
#include "marker.h"

class GLFont;

class BaseGraph: public QObject, public QLayoutItem
{
	Q_OBJECT
public:
	virtual void init();
	virtual void cleanup();
	virtual void validate();
	virtual void draw( double pos );

	virtual bool isVisible() const
	{
		return m_GraphState.getPrimaryBuffer().isVisible;
	}

	virtual GraphState getGraphState() const
	{
		return m_GraphState.getPrimaryBuffer();
	}

	virtual QRect getDataPos() const
	{
		return m_DataPos.getPrimaryBuffer();
	}

	virtual void addMarker( Marker * m );

	// Methoden von QLayoutItem
	virtual QSize sizeHint () const;
	virtual QSize minimumSize () const;
	virtual QSize maximumSize () const;
	virtual Qt::Orientations expandingDirections () const;
	virtual void setGeometry ( const QRect & r );
	virtual QRect geometry () const;
	virtual bool isEmpty () const;

	virtual ~BaseGraph() {}

public slots:
	virtual void setGrayLevel( unsigned char g );

protected:
	BaseGraph( QWidget * parent, int id );

	virtual void updateDiv();
	virtual void updateGrid();
	virtual void updateText();
	virtual void updateMarker(bool force);
	virtual void setDataViewport() = 0;
	virtual void setGraphViewport();
	virtual void drawData( double pos ) = 0;
	virtual void drawGrid();
	virtual void drawText();
	virtual void drawMarker();

	virtual double calculateDiv( double high, double low, int fracdigit, int maxlines );

	static const unsigned char c_ubgridalpha;

	struct TextObj
	{
		QString str;
		GLdouble x;
		GLdouble y;
	};

	struct Metrics
	{
		double xDiv, yDiv;
		double xRatio, yRatio;
		double xOffset, yOffset;
		int width, height;
	};

	DoubleBufferControl m_StateControl;
	DoubleBuffer<GraphState> m_GraphState;
	DoubleBuffer<CColor> m_AxisColor;

	DoubleBufferControl m_PosControl;
	DoubleBuffer<QRect> m_GraphPos;
	DoubleBuffer<QRect> m_DataPos;

	Q3PtrList< Marker > m_Markers;

	Metrics m_Metrics;

	GLFont * m_pGlFont;

	GLuint m_GridListBase;

	QWidget * m_Parent;
	const int m_id;
};

#endif //BASE_GRAPH_H
