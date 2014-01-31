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

#ifndef MARKER_H
#define MARKER_H

#include <qobject.h>

#include "graphdefs.h"
#include "doublebuffer.h"

#include <GL/glew.h>

class BaseGraph;

class Marker : public QObject
{
	Q_OBJECT
public:
	enum MarkerDirection { M_H, M_V };

public:
	Marker( BaseGraph * dest, GLint factor = 0, GLushort pattern = 0xAAAA, GLfloat width = 2);
	void init();
	void draw();
	void update(bool force);

public slots:
	void setValue( double val );
	void setVisible( bool b );
	void setDirection( int d );
	void setGrayLevel( unsigned char g );

private:
	DoubleBufferControl m_MarkerBufferControl;
	DoubleBuffer<double> m_Value;
	DoubleBuffer<bool> m_Visible;
	DoubleBuffer<MarkerDirection> m_Type;
	DoubleBuffer<unsigned char> m_GrayLevel;

	GLuint m_ListBase;

	BaseGraph * m_Destination;
	const GLint m_LinePatternFactor;
	const GLushort m_LinePattern;
	const GLfloat m_LineWidth;
};

#endif // MARKER_H
