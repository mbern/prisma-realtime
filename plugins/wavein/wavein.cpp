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

#include "wavein.h"
#include "dataobj.h"
#include "streamcontainer.h"

#include "config/node_macro.h"
//Added by qt3to4:
#include <QTimerEvent>

WaveIn::WaveIn()
: m_usedwavebuffers(NHDR),
  m_isrecording(false), m_isconfigured(false),
  m_pWaveBuffer(NULL), m_FrameRefCount(0),
  m_refcount(0), m_refLock(TRUE),
  m_WaveInControl("WaveDeviceConfiguration")
{
	m_wavindevice = 0;
	m_TimerId = 0;
}

WaveIn::~WaveIn()
{
    MMRESULT retvalue;

	stopRunning();

	retvalue = waveInClose( m_wavindevice );
	m_wavindevice = 0;
}

double WaveIn::getTime()
{
	return regression.interpolate();
}

bool WaveIn::configure(PluginNode * config)
{
	(void) config; //surpress warning

    MMRESULT retvalue;
	WAVEINCAPS waveincaps;

	// Return if the parent widget is not defined
	if( m_WaveInControl.parent == NULL )
		return FALSE;

	// Define the wave format the application uses
    m_wavformat.wFormatTag = WAVE_FORMAT_PCM;
    m_wavformat.nChannels = 2;
    m_wavformat.nSamplesPerSec = SAMPLEFREQ;
    m_wavformat.wBitsPerSample = 16;
    m_wavformat.nBlockAlign = ( m_wavformat.nChannels * m_wavformat.wBitsPerSample ) / 8;
    m_wavformat.nAvgBytesPerSec = m_wavformat.nSamplesPerSec * m_wavformat.nBlockAlign;
    m_wavformat.cbSize = 0;

	// Create the GUI controls
	OBJECTNODE_DECLARE_AND_GET(m_WaveInControl,WaveInControl,config);
	m_WaveInControl.widget = new WaveInControl( NODE_VARIABLE_NAME(m_WaveInControl), m_WaveInControl.parent );

	// Get the number of available devices. Return if no device is available
	uint numdevs = waveInGetNumDevs();
	if( numdevs == 0 )
	{
		qFatal("!!!! waveInGetNumDevs: No input devices available");
		return FALSE;
	}

	// Iterate over all available sound devices and register them if they support
	// the wave format
	for( uint i = 0; i < numdevs; i++ )
	{
		// Get the sound card capabilities
		retvalue = waveInGetDevCaps(i,&waveincaps,sizeof(waveincaps));
		if(retvalue != MMSYSERR_NOERROR)
		{
			qWarning( "!!!! waveInGetDevCaps: %s", translateMMError( retvalue ).ascii() );
			continue;
		}

		// Check if the device supports the required wave format
		if((waveincaps.dwFormats & WAVE_FORMAT_4S16) == 0)
			continue;

		// Add the sound cart to the list in the GUI
		m_WaveInControl.widget->addSoundcard(QString::fromUcs2((unsigned short*)waveincaps.szPname), i);
	}

	// Connect the GUI events with an appropriate action
	this->connect( m_WaveInControl.widget, SIGNAL(newSoundcardChosen(int)), SLOT(setupAudioDevice(int)));

	// Create the data buffer for the sound data
	m_pWaveBuffer = new DataBuffer(
		25,
		new StreamContainer(new DataObj< Q_INT16, true >( BUFFERSIZE >> 2, PC_STR_AUDIO_16BIT_MONO, PT_STR_SHORT ), NULL)
		);

	// Mark the module configured
	m_isconfigured = true;

	// Configure the wavein device according to the settings in the GUI
	m_WaveInControl.widget->fireConfiguration();

	return TRUE;
}

void WaveIn::setupAudioDevice( int id )
{
	MMRESULT retvalue;
	bool restart = false;

	// Debug crash
	//qDebug("Device ID: %d",id);

	// Check if a wave device is already open
	if( m_wavindevice != NULL )
	{
		// Don't change anything when we have the same device
		unsigned int previd;
		retvalue = waveInGetID(m_wavindevice,&previd);
		if(previd == (unsigned int)id)
			return;

		// Stop recording
		stopRunning();

		// Close the current wavein device
		retvalue = waveInClose(m_wavindevice);
		m_wavindevice = 0;
		if( retvalue != MMSYSERR_NOERROR )
			qFatal( "!!!! waveInClose: %s", translateMMError( retvalue ).ascii() );

		// Remember that we want to restart recording
		restart = true;
	}

	// Open the new wavein device
	retvalue = waveInOpen( &m_wavindevice, id, &m_wavformat, (DWORD_PTR)waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION );
    if( retvalue != MMSYSERR_NOERROR )
        qFatal( "!!!! waveInOpen: %s", translateMMError( retvalue ).ascii() );


	// Do we need to restart recording?
	if(restart)
	{
		// Start recording
		startRunning();
	}
}

bool WaveIn::stopRunning()
{
    MMRESULT retvalue;
	WAVEHDR * phdr;

	// Stop only if we are recording
	if(m_isrecording)
	{
		// Stop periodic timer
		this->killTimer(m_TimerId);

		// Notify the callback to stop recording
		m_isrecording = false;

		// Stop recording
		retvalue = waveInReset( m_wavindevice );
		if( retvalue != MMSYSERR_NOERROR )
			qFatal( "!!!! waveInReset: %s", translateMMError( retvalue ).ascii() );

		// Delete all buffers
		while(m_usedwavebuffers.pop_front(&phdr))
		{
			retvalue = waveInUnprepareHeader(m_wavindevice,phdr,sizeof(WAVEHDR));
			if( retvalue != MMSYSERR_NOERROR )
				qFatal( "!!!! waveInUnprepareHeader: %s", translateMMError( retvalue ).ascii() );
			delete[] phdr->lpData;
			delete phdr;
		}

		// Reset synchronisation
		regression.reset();

		return true;
	}
	else
		return false;
}

bool WaveIn::startRunning()
{
    MMRESULT retvalue;

	// Start only if we are configured and do not recording
	if( m_isconfigured && !m_isrecording )
	{
		// Create new buffers for recording and add them to the audio device
		WAVEHDR * pHdr = NULL;
		for( int i = 0; i < NHDR; i++ )
		{
			pHdr = new WAVEHDR;
			ZeroMemory(pHdr, sizeof(WAVEHDR));
			pHdr->lpData = new char[BUFFERSIZE];
			pHdr->dwBufferLength = BUFFERSIZE;

			retvalue = waveInPrepareHeader( m_wavindevice, pHdr, sizeof(WAVEHDR) );
			if( retvalue != MMSYSERR_NOERROR )
				qFatal( "!!!! waveInPrepareHeader: %s", translateMMError( retvalue ).ascii() );

			retvalue = waveInAddBuffer( m_wavindevice, pHdr, sizeof(WAVEHDR) );
			if( retvalue != MMSYSERR_NOERROR )
				qFatal( "!!!! waveInAddBuffer: %s", translateMMError( retvalue ).ascii() );
		}

		// Start recording
		m_FrameRefCount = 0;
		m_isrecording = true;

		retvalue = waveInStart( m_wavindevice );
		if( retvalue != MMSYSERR_NOERROR )
			qFatal( "!!!! waveInStart: %s", translateMMError( retvalue ).ascii() );

		// Do initial synchronisation
		regression.synchronize(m_wavindevice);

		// Start periodic timer
		m_TimerId = this->startTimer(1000);

		return true;
	}
	else
		return false;
}

QString WaveIn::name()
{
	return QString( "wavein" );
}


void WaveIn::handleData( UINT uMsg, DWORD dwParam )
{
	WAVEHDR * hdr;
	DWORD length;
	int channel;

    switch( uMsg )
    {
    case WIM_CLOSE:
        //qDebug( "WIM_CLOSE" );
        break;

    case WIM_OPEN:
        //qDebug( "WIM_OPEN" );
        break;

    case WIM_DATA:
        hdr = (WAVEHDR *)dwParam;

		if( m_isrecording )
		{
			length = hdr->dwBytesRecorded / 4;

			StreamContainer * pContainer = m_pWaveBuffer->getWriteObj(m_FrameRefCount,length,(double)1/(double)SAMPLEFREQ);
			PI_DataObj * out = pContainer->getItem(PC_IDX_AUDIO_16BIT_MONO);

			out->setLength( length );
			out->setType( PS_IDX_AUDIO | PC_IDX_AUDIO_16BIT_MONO );

			Q_INT16 * output = (Q_INT16*)out->dataPtr();
			Q_INT16 * source = (Q_INT16*)hdr->lpData;
			channel = m_WaveInControl.widget->getChannel();

			for( unsigned int i=0; i < out->length(); i++ )
			{
				output[i] = source[(i << 1) + channel];
			}

			m_pWaveBuffer->returnWriteObj( pContainer );

			m_FrameRefCount += length;
		}

		if(!m_usedwavebuffers.push_back(hdr))
			qWarning("usedwavebuffer full!!!");

        break;

    default:
		qFatal( "waveInProc: unexpected message" );
    }
}

void WaveIn::timerEvent( QTimerEvent * )
{
	MMRESULT retvalue;
	WAVEHDR * phdr;

	if(m_isrecording)
	{
		// Next synchronisation step to sound source
		regression.synchronize(m_wavindevice);

		// Add the available buffers to the sound device
		while(m_usedwavebuffers.pop_front(&phdr))
		{
			retvalue = waveInUnprepareHeader(m_wavindevice,phdr,sizeof(WAVEHDR));
			if( retvalue != MMSYSERR_NOERROR )
				qFatal( "!!!! waveInUnprepareHeader: %s", translateMMError( retvalue ).ascii() );

			phdr->dwFlags = 0;

			retvalue = waveInPrepareHeader(m_wavindevice,phdr,sizeof(WAVEHDR));
			if( retvalue != MMSYSERR_NOERROR )
				qFatal( "!!!! waveInPrepareHeader: %s", translateMMError( retvalue ).ascii() );

			retvalue = waveInAddBuffer(m_wavindevice,phdr,sizeof(WAVEHDR));
			if( retvalue != MMSYSERR_NOERROR )
				qFatal( "!!!! waveInAddBuffer: %s", translateMMError( retvalue ).ascii() );
		}
	}
}

QString WaveIn::translateMMError( MMRESULT error )
{
	WCHAR errormessage[MAXERRORLENGTH];
	MMRESULT retvalue;

	retvalue = waveInGetErrorText(error,errormessage,MAXERRORLENGTH);
	if(retvalue == MMSYSERR_NOERROR)
		return QString::fromUcs2((unsigned short*)errormessage);
	else
		return QString("Unknown error");
}


extern "C" {
PLUGIN_EXPORT void LoadObject( PI_Unknown ** ppUnknown )
{
	WaveIn *obj = new WaveIn();
	obj->queryInterface( PIID_UNKNOWN, (void**)ppUnknown );
}
} //extern "C"

WaveIn::regr::regr()
{
	param_set = 0;
	a[param_set] = 0.001;
	b[param_set] = - (double)timeGetTime() / 1000.0;

	reset();
}

void WaveIn::regr::synchronize(HWAVEIN hwi)
{
	MMTIME mmt;
	DWORD stime;
	DWORD ctime;
	DWORD diff;
	MMRESULT res;
	double newsample;
	double newtime;
	double det;
	int set;

	mmt.wType = TIME_BYTES;

	timeBeginPeriod(1);
	stime = timeGetTime();
	res = waveInGetPosition(hwi, &mmt, sizeof(MMTIME));
	ctime = timeGetTime();
	timeEndPeriod(1);

	if(res == MMSYSERR_NOERROR)
	{
		// Check if measurement was ok
		if(ctime - stime != 0 && n > 0)
		{
			return;
		}

		// Calculate new values
		if(n > 0)
		{
			diff = stime - lasttime;
			newtime = time[lastindex] + diff;
			diff = mmt.u.cb - lastsample;
			newsample = sample[lastindex] + diff/(double)(SAMPLEFREQ*4);
		}
		else
		{
			newtime = (double)stime;
			newsample = (double)mmt.u.cb/(double)(SAMPLEFREQ*4);
		}
		lasttime = stime;
		lastsample = mmt.u.cb;

		// Check if ring buffer is full or not
		if(n == REG_LENGTH)
		{
			// Remove oldest value pair from statistics
			suv -= time[index] * sample[index];
			su -= time[index];
			sv -= sample[index];
			su2 -= time[index] * time[index];
		}
		else
		{
			n++;
		}

		// Save new value pair in buffer
		time[index] = newtime;
		sample[index] = newsample;

		// Add new value pair to statistics
		suv += time[index] * sample[index];
		su += time[index];
		sv += sample[index];
		su2 += time[index] * time[index];

		// Do linear fit
		set = param_set ^ 0x1;
		if(n > 1)
		{
			det = n * su2 - su * su;
			a[set] = (n* suv - su * sv) / det;
			b[set] = (su2 * sv - suv * su) / det;
		}
		else
		{
			a[set] = 1.0/1000.0;
			b[set] = sample[index] - a[set] * time[index];
		}
		param_set = set;

		// Adjust index to buffer
		lastindex = index;
		index = (index + 1) % REG_LENGTH;
	}
	else
	{
		reset();
	}
}

double WaveIn::regr::interpolate()
{
	int set = param_set;
	return a[set] * timeGetTime() + b[set];
}

void WaveIn::regr::reset()
{
	suv = 0;
	su = 0;
	sv = 0;
	su2 = 0;
	n = 0;
	index = 0;
	lastindex = 0;
	lasttime = 0;
	lastsample = 0;
}
