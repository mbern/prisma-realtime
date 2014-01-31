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

#ifndef DATA_OBJ_H
#define DATA_OBJ_H

#include "pi_dataobj.h"

#include <qstring.h>

template< class T, bool F >
class DataObj : public PI_DataObj
{
public:
	DataObj( int size, QString name, QString typeName )
	: m_name( name ), m_typeName( typeName ), m_length( 0 ), m_allocatedLength(size), m_refNumber(0), m_type(0), m_refTime(0)
	{
		m_dataPtr = new T[size] ;
	}

	virtual ~DataObj()
	{
		delete[] m_dataPtr;
	}

	virtual Q_UINT64 refNumber()
	{
		return m_refNumber;
	}

	virtual double refTime()
	{
		return m_refTime;
	}

	virtual Q_UINT32 type()
	{
		return m_type;
	}

	virtual Q_UINT16 elementSize()
	{
		return sizeof( T );
	}

	virtual Q_UINT32 length()
	{
		return m_length;
	}

	virtual Q_UINT32 allocatedLength()
	{
		return m_allocatedLength;
	}

	virtual bool isInteger()
	{
		return F;
	}

	virtual QString name()
	{
		return m_name;
	}

	virtual QString typeName()
	{
		return m_typeName;
	}

	virtual void setRefNumber( Q_UINT64 number )
	{
		m_refNumber = number;
	}

	virtual void setRefTime( double time )
	{
		m_refTime = time;
	}

	virtual void setType( Q_UINT32 type )
	{
		m_type = type;
	}

	virtual void setLength( Q_UINT32 length )
	{
		m_length = length;
	}

	virtual void * dataPtr()
	{
		return m_dataPtr;
	}

	virtual PI_DataObj * createNewObj()
	{
		return new DataObj<T,F>(m_allocatedLength,m_name,m_typeName);
	}

private:
	const QString m_name;
	const QString m_typeName;

	Q_UINT32 m_length;
	const Q_UINT32 m_allocatedLength;

	Q_UINT64 m_refNumber;
	Q_UINT32 m_type;
	double m_refTime;

	T * m_dataPtr;
};

#endif //DATA_OBJ_H
