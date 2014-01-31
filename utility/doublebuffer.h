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

#ifndef DOUBLE_BUFFER_H
#define DOUBLE_BUFFER_H

#include <qmutex.h>
#include <q3ptrlist.h>

class DoubleBufferBase;

class DoubleBufferControl
{
	friend class DoubleBufferBase;
public:
	DoubleBufferControl();
	virtual ~DoubleBufferControl();
	virtual bool update();
	virtual void clear();
	virtual void set();
protected:
	virtual void registerBuffer(const DoubleBufferBase * buffer);
private:
	bool m_Flag;
	mutable QMutex m_Mutex;
	Q3PtrList<DoubleBufferBase> m_List;
};

class DoubleBufferBase
{
	friend class DoubleBufferControl;
public:
	DoubleBufferBase(DoubleBufferControl & control)
	{
		control.registerBuffer(this);
	}
	virtual ~DoubleBufferBase()
	{
	}
protected:
	virtual void copyBuffer() = 0;
};


template<class T>
class DoubleBuffer : public DoubleBufferBase
{
public:
	DoubleBuffer(DoubleBufferControl & control)
		: DoubleBufferBase(control)
	{
	}
	virtual ~DoubleBuffer()
	{
	}
	virtual inline const T & getPrimaryBuffer() const
	{
		return m_PrimaryBuffer;
	}
	virtual inline const T & getSecondaryBuffer() const
	{
		return m_SecondaryBuffer;
	}
	virtual inline void setSecondaryBuffer(T update)
	{
		m_SecondaryBuffer = update;
	}
protected:
	virtual void copyBuffer()
	{
		m_PrimaryBuffer = m_SecondaryBuffer;
	}
private:
	T m_PrimaryBuffer;
	T m_SecondaryBuffer;
};


#endif // DOUBLE_BUFFER_H
