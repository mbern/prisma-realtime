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

#include "doublebuffer.h"

#include <stdlib.h>


// DoubleBufferControl //

DoubleBufferControl::DoubleBufferControl()
: m_Flag(false)
{
}

DoubleBufferControl::~DoubleBufferControl()
{

}

bool DoubleBufferControl::update()
{
	QMutexLocker l(&m_Mutex);
	bool oldflag = m_Flag;
	if( m_Flag )
	{
		DoubleBufferBase * buffer;
		for(buffer = m_List.first(); buffer; buffer = m_List.next())
		{
			buffer->copyBuffer();
		}
	}
	m_Flag = false;
	return oldflag;
}

void DoubleBufferControl::clear()
{
	QMutexLocker l(&m_Mutex);
	m_Flag = false;
}

void DoubleBufferControl::set()
{
	QMutexLocker l(&m_Mutex);
	m_Flag = true;
}

void DoubleBufferControl::registerBuffer(const DoubleBufferBase * buffer)
{
	QMutexLocker l(&m_Mutex);
	m_List.append(buffer);
}
