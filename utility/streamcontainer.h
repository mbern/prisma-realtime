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

#ifndef STREAM_CONTAINER_H
#define STREAM_CONTAINER_H

#include "pi_dataobj.h"

class QMutex;

class StreamContainer
{
protected:
	StreamContainer(StreamContainer * sc);

public:
	StreamContainer( PI_DataObj * obj, ... );
	virtual ~StreamContainer();

	virtual int numItems()
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Return number of objects in list
		return obj->m_objlistlen;
	}

	virtual PI_DataObj * getItem(Q_UINT32 i)
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Check bounds and return object
		if(i < obj->m_objlistlen)
			return obj->m_objlist[i];
		else
			return NULL;
	}

	virtual Q_UINT64 getRefNumberStart()
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Return the appropriate value
		return obj->m_refNumber;
	}

	virtual Q_UINT64 getRefNumberEnd()
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Return the appropriate value
		return obj->m_refNumber + obj->m_length;
	}

	virtual Q_UINT32 getRefNumberLength()
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Return the appropriate value
		return obj->m_length;
	}

	virtual double getRefTimeStart()
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Return the appropriate value
		return (double)obj->m_refNumber * obj->m_refToTimeFactor;
	}

	virtual double getRefTimeEnd()
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Return the appropriate value
		return (double)(obj->m_refNumber + obj->m_length) * obj->m_refToTimeFactor;
	}

	virtual double getRefTimeLength()
	{
		// Get the pointer to the main object
		StreamContainer * obj = getMainObj();
		// Return the appropriate value
		return (double)obj->m_length * obj->m_refToTimeFactor;
	}

	virtual StreamContainer * createNewContainer();
	virtual StreamContainer * getReadObj();
	virtual StreamContainer * getWriteObj();
	virtual StreamContainer * getWriteObj( Q_UINT64 ref, Q_UINT32 len, double factor );

	virtual StreamContainer * getMainObj()
	{
		if(m_mainObj != NULL)
			return m_mainObj;
		else
			return this;
	}

protected:
	Q_UINT64 m_refNumber;
	Q_UINT32 m_length;
	double m_refToTimeFactor;

	PI_DataObj ** m_objlist;
	Q_UINT32 m_objlistlen;

	QMutex * m_mutex;
	Q_INT32 m_accessCounter;

	StreamContainer * m_mainObj;
};

#endif // STREAM_CONTAINER_H
