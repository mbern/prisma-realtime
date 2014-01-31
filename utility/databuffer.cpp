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

#include "databuffer.h"
//Added by qt3to4:
#include <Q3PtrList>


/// DataBuffer
DataBuffer::DataBuffer( int buffersize, StreamContainer* refobj )
{
	m_RefObj = refobj;
	m_BufferSize = buffersize;

	for( uint i = 0; i < m_BufferSize; i++ )
	{
		m_NotUsedObjList.append( m_RefObj->createNewContainer() );
	}
}

DataBuffer::~DataBuffer()
{
	StreamContainer * pobj;

	lockList();
	{
		Q3PtrListIterator< StreamContainer > it1( m_DataList );
		while( ( pobj = it1.current() ) != 0 )
		{
			++it1;
			delete pobj;
		}
	}
	unlockList();

	Q3PtrListIterator< StreamContainer > it2( m_NotUsedObjList );
	while( ( pobj = it2.current() ) != 0 )
	{
		++it2;
		delete pobj;
	}

	delete m_RefObj;
}

DataReader * DataBuffer::createDataReader()
{
	DataReader * tempobj = NULL;
	lockList();
	{
        tempobj = new DataReader( this, &m_DataList );
	}
	unlockList();

	return tempobj;
}

StreamContainer* DataBuffer::getWriteObj( Q_UINT64 ref, Q_UINT32 len, double factor )
{
	StreamContainer * tempobj = NULL;
	unsigned int datalen = 0;
	unsigned int poollen = 0;

	// Search the List for too old unused Buffers and remove them from m_DataList
	lockList();
	{
		bool flag = true;
		while( flag )
		{
			// Check if we can remove some objects
			if( m_DataList.count() > m_BufferSize )
			{
				// Retrieve the oldest object from the list
				tempobj = m_DataList.getFirst();
				// Try to get a write lock of this object
				StreamContainer * writeobj = tempobj->getWriteObj();
				if( writeobj != NULL )
				{
					// We are the only owner of this object an can move it to the pool
					m_DataList.removeFirst();
					m_NotUsedObjList.append( tempobj );
					// Now we release the write lock
					delete writeobj;
				}
				else
					// Give up
					flag = false;
			}
			else
				// Give up
				flag = false;

		}
		// Get the current number of objects in the list
		datalen = m_DataList.count();
	}
	unlockList();

	// Resize Pool
	poollen = m_NotUsedObjList.count();
	while( (poollen > 20) && ((poollen+datalen) > (1.5*m_BufferSize)) )
	{
		tempobj = m_NotUsedObjList.take();
		delete tempobj;
		poollen--;
	}

	// Take obj from data pool or create a new one
	tempobj = m_NotUsedObjList.take();
	if( tempobj == NULL )
	{
		tempobj = m_RefObj->createNewContainer();
	}

	// Return a write locked object with the requested timestamp
	return tempobj->getWriteObj(ref,len,factor);
}

void DataBuffer::returnWriteObj( StreamContainer * obj )
{
	// Get the pointer to the real object
	StreamContainer * tempobj = obj->getMainObj();
	// Release the write lock
	delete obj;

	lockList();
	{
		// Append the object as the newest object
		m_DataList.append( tempobj );
	}
	unlockList();
}

/// DataReader
DataReader::DataReader( DataBuffer * buffer, Q3PtrList< StreamContainer > * list )
: m_buffer( buffer ), m_list( list )
{
	m_itr = new Q3PtrListIterator< StreamContainer >( *list );
	m_itr->toLast();
}

DataReader::~DataReader()
{
	delete m_itr;
}

uint DataReader::getBufferSize()
{
	uint number = 0;

	m_buffer->lockList();
	{
		number = m_list->count();
	}
	m_buffer->unlockList();

	return number;
}

StreamContainer* DataReader::getCurrentObj()
{
	StreamContainer * mainobj = NULL;
	StreamContainer * readobj = NULL;

	m_buffer->lockList();
	{
		// Get the current object form the list
		mainobj = m_itr->current();
		// If valid, try to get an object with read lock
		if( mainobj != NULL )
			readobj = mainobj->getReadObj();
	}
	m_buffer->unlockList();

	return readobj;
}

StreamContainer* DataReader::getPreviousObj()
{
	StreamContainer * mainobj = NULL;
	StreamContainer * readobj = NULL;

	m_buffer->lockList();
	{
		// Try to get the previous object form the list
		if( ! m_itr->atFirst() )
			mainobj = --(*m_itr);
		else
			mainobj = NULL;
		// If valid, try to get an object with read lock
		if( mainobj != NULL )
		{
			readobj = mainobj->getReadObj();
			// Return to the next object if not successful
			if(readobj == NULL)
				++(*m_itr);
		}
	}
	m_buffer->unlockList();

	return readobj;
}

StreamContainer* DataReader::getNextObj()
{
	StreamContainer * mainobj = NULL;
	StreamContainer * readobj = NULL;

	m_buffer->lockList();
	{
		// Try to get the next object form the list
		if( ! m_itr->atLast() )
			mainobj = ++(*m_itr);
		else
			mainobj = NULL;
		// If valid, try to get an object with read lock
		if( mainobj != NULL )
		{
			readobj = mainobj->getReadObj();
			// Return to the previous object if not successful
			if(readobj == NULL)
				--(*m_itr);
		}
	}
	m_buffer->unlockList();

	return readobj;
}

StreamContainer* DataReader::getNewestObj()
{
	StreamContainer * mainobj = NULL;
	StreamContainer * readobj = NULL;

	m_buffer->lockList();
	{
		// Get the newest object from the list
		mainobj = m_itr->toLast();
		// If valid, try to get an object with read lock
		if( mainobj != NULL )
			readobj = mainobj->getReadObj();
	}
	m_buffer->unlockList();

	return readobj;
}
