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

#ifndef DUMMYANALYSE_H
#define DUMMYANALYSE_H

#include "plugin_global.h"

#include "pi_configuration.h"
#include "pi_unknown.h"
#include "pi_dataobj.h"
#include "pi_guicontainer.h"
#include "pi_databuffer.h"

#include "databuffer.h"

#include <qthread.h>
#include <qmutex.h>

class DummyAnalyse : public QThread, public PI_Configuration,
	public PI_DataBufferProvider, public PI_DataBufferConsumer,
		virtual public PI_Unknown
{
public:
	DummyAnalyse();
	virtual ~DummyAnalyse();
	virtual void run();

	// Begin PI_Configuration
	virtual bool configure(PluginNode * config);
	virtual bool stopRunning();
	virtual bool startRunning();
	// Begin PI_Configuration

	// Begin PI_DataBufferProvider
	virtual QStringList getBufferList()
	{
		QStringList strlist;
		strlist << "compound";
		return strlist;
	}

	virtual DataReader * getDataReader(QString str)
	{
		if( str.lower() == "compound" )
		return m_pCompoundBuffer->createDataReader();
		else
		return NULL;
	}
	// End PI_DataBufferProvider

	// Begin PI_DataBufferConsumer
	virtual void connectToProvider( PI_DataBufferProvider * provider )
	{
		if(provider)
		{
			m_pProvider = provider;
			m_pWaveReader = m_pProvider->getDataReader( "wave" );
		}
	}
	// End PI_DataBufferConsumer

	// Begin PI_Unknown
	virtual QString name()
	{
		return QString("dummy_analyse");
	}
	virtual int queryInterface(const QUuid piid, void ** ppInterface)
	{
		QMutexLocker l(&m_refLock);
		if (piid == PIID_UNKNOWN)
		{
			*ppInterface = (PI_Unknown*)this;
			addRef();
		}
		else if (piid == PIID_CONFIGURATION)
		{
			*ppInterface = (PI_Configuration*)this;
			addRef();
		}
		else if (piid == PIID_DATA_BUFFER_PROVIDER)
		{
			*ppInterface = (PI_DataBufferProvider*)this;
			addRef();
		}
		else if (piid == PIID_DATA_BUFFER_CONSUMER)
		{
			*ppInterface = (PI_DataBufferConsumer*)this;
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
		if ( !--m_refcount)
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
private:
	void process();
	void rfft(double X[],int N,int M);

	static const Q_UINT32 c_fs;
	static const Q_UINT32 c_fftorder;
	static const Q_UINT32 c_nfft;
	static const Q_UINT32 c_ncenter;
	static const Q_UINT32 c_hop;
	static const Q_UINT32 c_nframe;

	double * m_buffer;
	double * m_bfft;
	double * m_window;
	double * m_ampl;

	DataBuffer * m_pCompoundBuffer;

	DataReader * m_pWaveReader;
	PI_DataBufferProvider * m_pProvider;

	Q_UINT64 m_OutFrameRefCount;

	bool m_isrunning;

	int m_refcount;
	QMutex m_refLock;
};

#endif /*DUMMYANALYSE_H_*/
