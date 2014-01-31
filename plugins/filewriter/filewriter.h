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

#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include "plugin_global.h"
#include "prisma_defines.h"

#include <qstring.h>
#include <quuid.h>
#include <qobject.h>
#include <qwidget.h>
#include <qbuffer.h>

#include "pi_configuration.h"
#include "pi_databuffer.h"
#include "pi_timehandler.h"
#include "pi_guicontainer.h"
#include "pi_unknown.h"

#include "databuffer.h"
#include "filewritecontrol.h"
#include "widgetinfo.h"

/************************************
 * Needs to be incremented for every 
 * format change of the bin file
 ***********************************/
#define PRISMA_BIN_VERSION 2

class StreamContainer;

class FileWriter : public QObject,
				   public PI_Configuration,
				   public PI_DataBufferConsumer,
				   public PI_TimeConsumer,
				   public PI_GuiContainer,
				   virtual public PI_Unknown
{
	Q_OBJECT

public:

	FileWriter();
	~FileWriter();

	// Begin PI_DataBufferConsumer //
	virtual void connectToProvider( PI_DataBufferProvider * provider )
	{
		DataReader * tempreader;

		// Abort if provider is NULL
		if(provider == NULL)
			return;

		//Try to connect to the spectral data reader
		tempreader = provider->getDataReader(PS_STR_ANALYSIS);
		if(tempreader != NULL)
		{
			m_pDataBufferCompoundProvider = provider;
			m_pCompoundReader = tempreader;
		}

		//Try to connect to the waveform data reader
		tempreader = provider->getDataReader(PS_STR_AUDIO);
		if(tempreader != NULL)
		{
			m_pDataBufferWaveProvider = provider;
			m_pWaveReader = tempreader;
		}
	}
	// End PI_DataBufferConsumer //

	// Begin PI_TimeConsumer //
	virtual void connectToProvider( PI_TimeProvider * provider )
	{
		m_pTimeProvider = provider;
	}
	// End PI_TimeConsumer //

	// Begin PI_Unknown //
	virtual QString name()
	{
		return QString( "filewriter" );
	}

	virtual int queryInterface( const QUuid piid, void ** ppInterface )
	{
		QMutexLocker l(&m_refLock);
		if( piid == PIID_UNKNOWN )
		{
		    *ppInterface = (PI_Unknown*)this;
		    addRef();
		}
		else if( piid == PIID_CONFIGURATION )
		{
			*ppInterface = (PI_Configuration*)this;
			addRef();
		}
		else if( piid == PIID_DATA_BUFFER_CONSUMER )
		{
			*ppInterface = (PI_DataBufferConsumer*)this;
			addRef();
		}
		else if( piid == PIID_TIME_CONSUMER )
		{
			*ppInterface = (PI_TimeConsumer*)this;
			addRef();
		}
		else if( piid == PIID_GUI_CONTAINER )
		{
			*ppInterface = (PI_GuiContainer*)this;
			addRef();
		}
		else
		{
            *ppInterface = NULL;
			return -1;
		}
		return 0;
	}

	virtual int release()
	{
		int tempcount;
		m_refLock.lock();
		if( !--m_refcount )
		{
			m_refLock.unlock();
			delete this;
			return 0;
		}
		tempcount = m_refcount;
		m_refLock.unlock();
		return tempcount;
	}

	virtual int addRef()
	{
		QMutexLocker l(&m_refLock);
		m_refcount++;
		return m_refcount;
	}
	// End PI_Unknown //

	// Begin PI_Configuration //
	virtual bool configure(PluginNode * config);
	virtual bool stopRunning();
	virtual bool startRunning();
	// End PI_Configuration //

	// Begin PI_GuiContainer //
	virtual void setParentWindow( QWidget * pParent, const QString & widget )
	{
		if(widget == m_FileWriteControl.name)
			m_FileWriteControl.parent = pParent;
	}

	virtual void connectSlot( const QString & signal, const QObject * obj, const QString & slot );
	virtual QObject * getObject( const QString & obj )
	{
		if( obj == m_FileWriteControl.name)
			return m_FileWriteControl.widget;
		else
			return NULL;
	}
	virtual QWidget * getWidget( const QString & widget )
	{
		if( widget == m_FileWriteControl.name )
			return m_FileWriteControl.widget;
		else
			return NULL;
	}
	// End PI_GuiContainer //

public slots:
	void saveDataBuffer( QString filename );
	void saveWaveBuffer( QString filename );
	void copyBuffer();
	void updateBuffer();
	void setBufferLength( int length );

signals:
	void bufferFilling( int );

private:

    int m_refcount;
	QMutex m_refLock;

	DataReader * m_pCompoundReader;
	DataReader * m_pWaveReader;

	double m_BufferLength;

	bool m_TailCaptured;
	bool m_TailSynchronized;
	StreamContainer * m_pCompoundTail;
	StreamContainer * m_pWaveTail;

	PI_TimeProvider * m_pTimeProvider;
	PI_DataBufferProvider * m_pDataBufferWaveProvider;
	PI_DataBufferProvider * m_pDataBufferCompoundProvider;

	QBuffer m_DataBuffer;
	QBuffer m_WaveBuffer;

	WidgetInfo<FileWriteControl> m_FileWriteControl;

	int timer_id;
};

#endif // FILE_WRITER_H
