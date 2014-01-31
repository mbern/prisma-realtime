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

#include "dummy_wavein.h"
#include "dataobj.h"
#include "streamcontainer.h"

#ifdef Q_OS_WIN
#define _USE_MATH_DEFINES
#endif //Q_OS_WIN
#include <math.h>
//Added by qt3to4:
#include <QTimerEvent>

DummyWaveIn::DummyWaveIn() :
	m_TimerId(0), m_isrecording(false), m_pWaveBuffer(NULL),
			m_FrameRefCount(0), m_refcount(0), m_refLock(TRUE)
{
	starttime.start();
}

DummyWaveIn::~DummyWaveIn()
{
	stopRunning();
	if(m_pWaveBuffer)
		delete m_pWaveBuffer;
}

bool DummyWaveIn::configure(PluginNode * config)
{
	(void) config; //surpress warning

	// Create the data buffer for the sound data
	m_pWaveBuffer = new DataBuffer(
			25,
			new StreamContainer(new DataObj< Q_INT16, true >( BUFFERSIZE, PC_STR_AUDIO_16BIT_MONO, PT_STR_SHORT ), NULL)
	);

	return TRUE;
}

bool DummyWaveIn::stopRunning()
{
	// Stop only if we are recording
	if (m_isrecording)
	{
		// Stop periodic timer
		this->killTimer(m_TimerId);

		// Notify the callback to stop recording
		m_isrecording = false;

		// Reset synchronisation
		approximation.reset();

		return true;
	}
	else
		return false;
}

bool DummyWaveIn::startRunning()
{
	// Start only if we are configured and do not recording
	if (!m_isrecording)
	{
		// Start recording
		m_FrameRefCount = 0;
		m_isrecording = true;

		// Do initial synchronisation
		approximation.reset();
		starttime.restart();

		// Start periodic timer
		m_TimerId = this->startTimer(BUFFERMS);

		return true;
	}
	else
		return false;
}

double DummyWaveIn::getTime()
{
	return starttime.elapsed() / (double)1000;
}

void DummyWaveIn::timerEvent(QTimerEvent * e)
{
	(void) e;

	if (m_isrecording)
	{
		while(starttime.elapsed() >= (int)(m_FrameRefCount * 1000 / SAMPLEFREQ) )
		{
			StreamContainer * pContainer = m_pWaveBuffer->getWriteObj(
					m_FrameRefCount, BUFFERSIZE, 1.0/(double)SAMPLEFREQ);

			PI_DataObj * out = pContainer->getItem(PC_IDX_AUDIO_16BIT_MONO);
			out->setLength( BUFFERSIZE );
			out->setType( PS_IDX_AUDIO | PC_IDX_AUDIO_16BIT_MONO );

			Q_INT16 * ptr = (Q_INT16*)out->dataPtr();

			for (int i = 0; i < BUFFERSIZE; i++)
			{
				ptr[i] = (Q_INT16)(approximation.next() * (1 << 13));
			}

			m_pWaveBuffer->returnWriteObj(pContainer);

			m_FrameRefCount += BUFFERSIZE;
		}
	}
}

extern "C" {
PLUGIN_EXPORT void LoadObject( PI_Unknown ** ppUnknown )
{
	DummyWaveIn *obj = new DummyWaveIn();
	obj->queryInterface( PIID_UNKNOWN, (void**)ppUnknown );
}
} //extern "C"

DummyWaveIn::sineapprox::sineapprox()
{
	reset();
}

void DummyWaveIn::sineapprox::reset()
{
	for(int i = 0; i < HARMONICS; i++)
	{
		double b = (i+1) * (double)BASEFREQ/(double)SAMPLEFREQ * 2.0 * M_PI;
		double d = i * 2.0 * M_PI / (double)HARMONICS;
		c[i] = 2.0 * cos(b);
		snm2[i] = sin(d + b);
		snm1[i] = sin(d + 2.0 * b);
		a[i] = 1.0/((i+1)*(i+1));
	}
}

double DummyWaveIn::sineapprox::next()
{
	double r = 0;
	for(int i = 0; i < HARMONICS; i++)
	{
		double s = c[i] * snm1[i] - snm2[i];
		snm2[i] = snm1[i];
		snm1[i] = s;

		r += s * a[i];
	}
	return r;
}
