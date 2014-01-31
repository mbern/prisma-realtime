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

#include "streamcontainer.h"

#include <stdarg.h>

#include <qmutex.h>

typedef PI_DataObj * pPI_DataObj;

StreamContainer::StreamContainer(StreamContainer * sc)
: m_objlist(NULL), m_mutex(NULL), m_mainObj(sc)
{
	m_accessCounter = 0;
	m_refNumber = 0;
	m_length = 0;
	m_refToTimeFactor = 0;
	m_objlistlen = 0;
}

StreamContainer::StreamContainer( PI_DataObj * obj, ... )
	: m_mainObj(NULL)
{
	m_mutex = new QMutex();
	m_accessCounter = 0;
	m_refNumber = 0;
	m_length = 0;
	m_refToTimeFactor = 0;

	va_list obj_Arg;

	// Count params
	va_start(obj_Arg,obj);
	m_objlistlen = 1;
	while(va_arg(obj_Arg,pPI_DataObj) != NULL)
	{
        m_objlistlen++;
	}
	va_end(obj_Arg);

	// Allocate array
	m_objlist = new pPI_DataObj[m_objlistlen];

	// Copy the first param
	m_objlist[0] = obj;

	// Copy the rest of the params
	va_start(obj_Arg,obj);
	for(Q_UINT32 i = 1; i < m_objlistlen; i++)
	{
		m_objlist[i] = va_arg(obj_Arg,pPI_DataObj);
	}
	va_end(obj_Arg);
}

StreamContainer::~StreamContainer()
{
	// Check which object we delete
	if(m_mainObj == NULL)
	{
		// Delete the elements in the list
		for(Q_UINT32 i = 0; i < m_objlistlen; i++)
		{
			if(m_objlist[i] != NULL)
				delete m_objlist[i];
		}
		// Delete the list itself
		delete[] m_objlist;
		// Delete the mutex
		delete m_mutex;
	}
	else
	{
		// Make atomic access to main object
		QMutexLocker(m_mainObj->m_mutex);
		// Adjust the access counter
		if(m_mainObj->m_accessCounter > 0)
			m_mainObj->m_accessCounter--;
		else
			m_mainObj->m_accessCounter = 0;
	}
}

StreamContainer * StreamContainer::createNewContainer()
{
	// Proceed only if we are a main object
	if(m_mainObj != NULL)
		return NULL;

	// Create a new object and create also a new list
	StreamContainer * sc = new StreamContainer((StreamContainer*)NULL);
	sc->m_mutex = new QMutex();
	sc->m_objlist = new pPI_DataObj[this->m_objlistlen];
	sc->m_objlistlen = m_objlistlen;
	for(Q_UINT32 i = 0; i < this->m_objlistlen; i++)
	{
		sc->m_objlist[i] = this->m_objlist[i]->createNewObj();
	}
	return sc;
}

StreamContainer * StreamContainer::getReadObj()
{
	// Proceed only if we are a main object
	if(m_mainObj != NULL)
		return NULL;

	// Make this call atomic
	QMutexLocker locker(m_mutex);

	// Get read access to this object
	if(m_accessCounter >= 0)
		m_accessCounter++;
	else
		return NULL;

	// Return a new object to read from
	return new StreamContainer(this);
}

StreamContainer * StreamContainer::getWriteObj()
{
	return getWriteObj(0,0,0);
}

StreamContainer * StreamContainer::getWriteObj( Q_UINT64 ref, Q_UINT32 len, double factor )
{
	// Proceed only if we are a main object
	if(m_mainObj != NULL)
		return NULL;

	// Make this call atomic
	QMutexLocker locker(m_mutex);

	// Return if an object with write access exists already
	if(m_accessCounter < 0)
		return NULL;

	// Try to get write access to this object
	if(m_accessCounter == 0)
		m_accessCounter = -1;
	else
		return NULL;

	// OK, can now set the timestamp to the main object
	m_refNumber = ref;
	m_length = len;
	m_refToTimeFactor = factor;

	// Return a new object to write to
	return new StreamContainer(this);
}
