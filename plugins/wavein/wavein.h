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

#ifndef WAVEIN_H
#define WAVEIN_H

#include "plugin_global.h"
#include "prisma_defines.h"

#include <windows.h>

#include "pi_configuration.h"
#include "pi_unknown.h"
#include "pi_databuffer.h"
#include "pi_timehandler.h"
#include "pi_guicontainer.h"

#include "databuffer.h"
#include "waveincontrol.h"
#include "widgetinfo.h"
#include "safebuffer.h"

#include <qwidget.h>
#include <qobject.h>
#include <qmutex.h>
//Added by qt3to4:
#include <QTimerEvent>

#define SAMPLEFREQ 44100
#define NHDR 40
#define BUFFERSIZE (DWORD)(SAMPLEFREQ*4* 100 /1000)
#define REG_LENGTH 20


class PI_DataObj;

class WaveIn : public QObject,
			   public PI_Configuration,
			   public PI_DataBufferProvider,
			   public PI_TimeProvider,
			   public PI_GuiContainer,
			   virtual public PI_Unknown
{
	Q_OBJECT
public:
	WaveIn();

	virtual ~WaveIn();

	// Begin PI_GuiContainer
	virtual void setParentWindow( QWidget * pParent, const QString & widget )
	{
		if( widget == m_WaveInControl.name )
			m_WaveInControl.parent = pParent;
	}

	virtual void connectSlot( const QString & signal, const QObject * obj, const QString & slot )
	{
		(void) signal; // surpress warning
		(void) obj; // surpress warning
		(void) slot; // surpress warning
	}

	virtual QObject * getObject( const QString & obj )
	{
		if( obj == m_WaveInControl.name )
			return m_WaveInControl.widget;
		return NULL;
	}
	virtual QWidget * getWidget( const QString & widget )
	{
		if( widget == m_WaveInControl.name )
			return m_WaveInControl.widget;
		return NULL;
	}
	// End PI_GuiContainer

	// Begin PI_TimeProvider
	virtual double getTime();
	// End PI_TimeProvider

	// Begin PI_Configuration
	virtual bool configure(PluginNode * config);
	virtual bool stopRunning();
	virtual bool startRunning();
	// Begin PI_Configuration

	// Begin PI_DataBufferProvider
	virtual QStringList getBufferList()
	{
		QStringList strlist;
		strlist << PS_STR_AUDIO;
		return strlist;
	}

	virtual DataReader * getDataReader( QString str )
	{
		if( str.lower() == PS_STR_AUDIO )
			return m_pWaveBuffer->createDataReader();
		else
			return NULL;
	}
	// End PI_DataBufferProvider

	// Begin PI_Unknown
	virtual QString name();
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
		else if( piid == PIID_DATA_BUFFER_PROVIDER )
		{
			*ppInterface = (PI_DataBufferProvider*)this;
			addRef();
		}
		else if( piid == PIID_TIME_PROVIDER )
		{
			*ppInterface = (PI_TimeProvider*)this;
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
		//qDebug( "addRef: WaveIn reference count: %d", m_refcount );
		return m_refcount;
	}
	// End PI_Unknown

public slots:
	virtual void setupAudioDevice( int id );

protected:
	virtual void handleData( UINT uMsg, DWORD dwParam );
	void timerEvent(QTimerEvent * e);
private:
	static void CALLBACK waveInProc( HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
	{
		(void) hwi; // surpress warning
		(void) dwParam2; // surpress warning

		( (WaveIn*)dwInstance )->handleData( uMsg, dwParam1 );
	}

	class regr
	{
	public:
		regr();
		double interpolate();
		void reset();
		void synchronize(HWAVEIN hwi);
	private:
		double time[REG_LENGTH];
		double sample[REG_LENGTH];
		DWORD lasttime;
		DWORD lastsample;
		DWORD lastindex;
		DWORD index;
		double a[2];
		double b[2];
		int param_set;
		double suv;
		double su;
		double sv;
		double su2;
		double n;
	};

	SafeBuffer<WAVEHDR*> m_usedwavebuffers;

	QString translateMMError( MMRESULT error );

	int m_TimerId;

    HWAVEIN m_wavindevice;
	WAVEFORMATEX m_wavformat;

    bool m_isrecording;
	bool m_isconfigured;

	DataBuffer * m_pWaveBuffer;
	Q_UINT64 m_FrameRefCount;

    int m_refcount;
	QMutex m_refLock;

	regr regression;

	WidgetInfo<WaveInControl> m_WaveInControl;
};

#endif // WAVEIN_H
