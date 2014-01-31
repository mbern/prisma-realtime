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

#ifndef DATA_BUFFER
#define DATA_BUFFER

#include <q3ptrlist.h>
#include <qmutex.h>

#include "pi_dataobj.h"

#include "streamcontainer.h"

class DataReader;

class DataBuffer
{
public:
	DataBuffer( int buffersize, StreamContainer* refobj );
	virtual ~DataBuffer();

	virtual DataReader * createDataReader();

	virtual StreamContainer* getWriteObj( Q_UINT64 ref, Q_UINT32 len, double factor );
	virtual void returnWriteObj( StreamContainer * obj );

	virtual void lockList() { m_lock.lock(); }
	virtual void unlockList() { m_lock.unlock(); }

private:

	Q3PtrList< DataReader > m_ReaderList;

	Q3PtrList< StreamContainer > m_DataList;
	Q3PtrList< StreamContainer > m_NotUsedObjList;

	StreamContainer * m_RefObj;
	uint m_BufferSize;

	QMutex m_lock;

	unsigned int m_CurrentTime;
};

class DataReader
{
public:
	DataReader( DataBuffer * buffer, Q3PtrList< StreamContainer > * list );
	virtual ~DataReader();

	virtual uint getBufferSize();
	virtual StreamContainer* getCurrentObj();
	virtual StreamContainer* getPreviousObj();
	virtual StreamContainer* getNextObj();
	virtual StreamContainer* getNewestObj();
private:
	DataBuffer * m_buffer;
	Q3PtrList< StreamContainer > * m_list;
	Q3PtrListIterator< StreamContainer > * m_itr;
};

#endif // DATA_BUFFER
