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

#ifndef DUMMY_WAVEIN_H
#define DUMMY_WAVEIN_H

#include "plugin_global.h"
#include "prisma_defines.h"

#include "pi_unknown.h"
#include "pi_databuffer.h"
#include "pi_timehandler.h"
#include "pi_configuration.h"

#include <qobject.h>
#include <qmutex.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <QTimerEvent>

#include "databuffer.h"

#define BASEFREQ 880
#define HARMONICS 10
#define SAMPLEFREQ 44100
#define BUFFERMS 100
#define BUFFERSIZE (SAMPLEFREQ * BUFFERMS /1000)

class DummyWaveIn : public QObject, public PI_DataBufferProvider,
	public PI_TimeProvider, public PI_Configuration, virtual public PI_Unknown
{
Q_OBJECT
public:
	DummyWaveIn();
	virtual ~DummyWaveIn();

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

	virtual DataReader * getDataReader(QString str)
	{
		if (str.lower() == PS_STR_AUDIO)
			return m_pWaveBuffer->createDataReader();
		else
			return NULL;
	}
	// End PI_DataBufferProvider

	// Begin PI_Unknown
	virtual QString name()
	{
		return QString("dummy_wavein");
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
		else if (piid == PIID_TIME_PROVIDER)
		{
			*ppInterface = (PI_TimeProvider*)this;
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

protected:
	void timerEvent(QTimerEvent * e);

private:

	struct sineapprox {
		sineapprox();
		void reset();
		double next();
		double snm1[HARMONICS];
		double snm2[HARMONICS];
		double c[HARMONICS];
		double a[HARMONICS];
	} approximation;

	int m_TimerId;

    bool m_isrecording;

    QTime starttime;

	DataBuffer * m_pWaveBuffer;
	Q_UINT64 m_FrameRefCount;

	int m_refcount;
	QMutex m_refLock;
};

#endif /*DUMMY_WAVEIN_H*/
