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

#include <portaudio.h>

#include "pi_configuration.h"
#include "pi_unknown.h"
#include "pi_databuffer.h"
#include "pi_timehandler.h"
#include "pi_guicontainer.h"

#include "databuffer.h"
#include "portaudio_waveincontrol.h"
#include "widgetinfo.h"
#include "safebuffer.h"

#include <qwidget.h>
#include <qobject.h>
#include <qmutex.h>

#define SAMPLEFREQ 44100
#define FRAMES_PER_BUFFER 1000


class PI_DataObj;

class PaWaveIn : public QObject,
				 public PI_Configuration,
				 public PI_DataBufferProvider,
				 public PI_TimeProvider,
				 public PI_GuiContainer,
				 virtual public PI_Unknown
{
	Q_OBJECT
public:
	PaWaveIn();

	virtual ~PaWaveIn();

	// Begin PI_GuiContainer
	virtual void setParentWindow( QWidget * pParent, const QString & widget )
	{
		if( widget == m_PaWaveInControl.name )
			m_PaWaveInControl.parent = pParent;
	}

	virtual void connectSlot( const QString & signal, const QObject * obj, const QString & slot )
	{
		(void) signal; // surpress warning
		(void) obj; // surpress warning
		(void) slot; // surpress warning
	}

	virtual QObject * getObject( const QString & obj )
	{
		if( obj == m_PaWaveInControl.name )
			return m_PaWaveInControl.widget;
		return NULL;
	}
	virtual QWidget * getWidget( const QString & widget )
	{
		if( widget == m_PaWaveInControl.name )
			return m_PaWaveInControl.widget;
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
		//qDebug( "addRef: PaWaveIn reference count: %d", m_refcount );
		return m_refcount;
	}
	// End PI_Unknown

public slots:
	virtual void setupAudioDevice( int id );

protected:
	virtual int handleData(const void * data, unsigned long frameCount,
			const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags);

private:
	static int pa_stream_callback(const void *input, void *output, unsigned long frameCount,
			const PaStreamCallbackTimeInfo *timeInfo,
			PaStreamCallbackFlags statusFlags, void *userData)
	{
		(void) output; // surpress warning
		return ((PaWaveIn*)userData)->handleData(input, frameCount, timeInfo, statusFlags);
	}

	PaStream * m_audiostream;
	PaStreamParameters m_wavformat;

	bool m_isrecording;
	bool m_isconfigured;

	DataBuffer * m_pWaveBuffer;

	int m_refcount;
	QMutex m_refLock;

	WidgetInfo<PaWaveInControl> m_PaWaveInControl;
};

#endif // WAVEIN_H
