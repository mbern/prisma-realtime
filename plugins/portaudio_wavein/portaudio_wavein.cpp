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

#include "portaudio_wavein.h"
#include "dataobj.h"
#include "streamcontainer.h"

#include "config/node_macro.h"

PaWaveIn::PaWaveIn()
: m_isrecording(false), m_isconfigured(false),
  m_pWaveBuffer(NULL),
  m_refcount(0), m_refLock(TRUE),
  m_PaWaveInControl("WaveDeviceConfiguration")
{
	m_audiostream = NULL;
}

PaWaveIn::~PaWaveIn()
{
	if(m_audiostream != NULL)
	{
		stopRunning();

		Pa_CloseStream(m_audiostream);
		m_audiostream = NULL;
	}
}

double PaWaveIn::getTime()
{
	return Pa_GetStreamTime(m_audiostream);
}

bool PaWaveIn::configure(PluginNode * config)
{
	PaError err;
	PaDeviceIndex devidx;
	PaStreamParameters streampram;

	// Return if the parent widget is not defined
	if( m_PaWaveInControl.parent == NULL )
		return FALSE;

	// Create the GUI controls
	OBJECTNODE_DECLARE_AND_GET(m_PaWaveInControl,PaWaveInControl,config);
	m_PaWaveInControl.widget = new PaWaveInControl( NODE_VARIABLE_NAME(m_PaWaveInControl), m_PaWaveInControl.parent );

	// Initialize portaudio
	err = Pa_Initialize();
	if(err != paNoError)
		qFatal("Unable to initialize portaudio!");

	// Check if audio devices are available
	devidx = Pa_GetDeviceCount();
	if(devidx == 0)
		qFatal("No devices available!");

	// Select the capable devices
	for(PaDeviceIndex i = 0; i < devidx; i++)
	{
		const PaDeviceInfo * devinfo = Pa_GetDeviceInfo(i);

		// Discard input if it is not stereo
		// TODO: change that!!!
		if(devinfo == NULL || devinfo->maxInputChannels < 2)
			continue;

		// Fill in the format the application uses
		streampram.device = i;
		streampram.hostApiSpecificStreamInfo = NULL;
		streampram.sampleFormat = paInt16;
		streampram.suggestedLatency = 0;
		streampram.channelCount = 2;

		err = Pa_IsFormatSupported(&streampram, NULL, SAMPLEFREQ);
		if(err == paFormatIsSupported)
		{
			// Add the sound cart to the list in the GUI
			m_PaWaveInControl.widget->addSoundcard(devinfo->name, i);
		}
	}

	// Connect the GUI events with an appropriate action
	this->connect( m_PaWaveInControl.widget, SIGNAL(newSoundcardChosen(int)), SLOT(setupAudioDevice(int)));

	// Create the data buffer for the sound data
	m_pWaveBuffer = new DataBuffer(
		25,
		new StreamContainer(new DataObj< Q_INT16, true >( FRAMES_PER_BUFFER, PC_STR_AUDIO_16BIT_MONO, PT_STR_SHORT ), NULL)
		);

	// Initialize the wave format structure
	m_wavformat.device = paNoDevice;

	// Mark the module configured
	m_isconfigured = true;

	// Configure the wavein device according to the settings in the GUI
	m_PaWaveInControl.widget->fireConfiguration();

	return TRUE;
}

void PaWaveIn::setupAudioDevice( int id )
{
	PaError err;
	bool restart = false;

	// Retrieve sound device information
	const PaDeviceInfo * devinfo = Pa_GetDeviceInfo(id);
	if(devinfo == NULL)
		qFatal("Unknown audio device");

	// Check if a wave device is already open
	if( m_audiostream != NULL )
	{
		// Don't change anything when we have the same device
		if(m_wavformat.device == id)
			return;

		// Stop recording
		stopRunning();

		// Close the current wavein device
		err = Pa_CloseStream(m_audiostream);
		m_audiostream = NULL;
		m_wavformat.device = paNoDevice;
		if( err != paNoError )
			qFatal( "Unable to close input stream" );

		// Remember that we want to restart recording
		restart = true;
	}

	// Fill in the format the application uses
	m_wavformat.device = id;
	m_wavformat.hostApiSpecificStreamInfo = NULL;
	m_wavformat.sampleFormat = paInt16;
	m_wavformat.suggestedLatency = devinfo->defaultHighInputLatency;
	m_wavformat.channelCount = 2;

	// Open the audio device
	err = Pa_OpenStream(&m_audiostream, &m_wavformat, NULL, SAMPLEFREQ,
			paFramesPerBufferUnspecified, paNoFlag, PaWaveIn::pa_stream_callback, this);
	if(err != paNoError)
		qFatal("Unable to open audio stream");

	// Do we need to restart recording?
	if(restart)
	{
		// Start recording
		startRunning();
	}
}

bool PaWaveIn::stopRunning()
{
	PaError err;

	// Stop only if we are recording
	if(m_isrecording)
	{
		// Notify the callback to stop recording
		m_isrecording = false;

		// Stop recording
		err = Pa_StopStream(m_audiostream);
		if(err != paNoError)
			qFatal("Unable to stop audio stream");

		return true;
	}
	else
		return false;
}

bool PaWaveIn::startRunning()
{
	PaError err;

	// Start only if we are configured and do not recording
	if( m_isconfigured && !m_isrecording )
	{
		// Start recording
		m_isrecording = true;

		err = Pa_StartStream(m_audiostream);
		if(err != paNoError)
			qFatal("Unable to start audio stream");

		return true;
	}
	else
		return false;
}

QString PaWaveIn::name()
{
	return QString( "portaudio_wavein" );
}


int PaWaveIn::handleData(const void * data, unsigned long frameCount,
			const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
{
	(void)timeInfo;
	(void)statusFlags;

	if( m_isrecording )
	{
		StreamContainer * pContainer = m_pWaveBuffer->getWriteObj(
				timeInfo->inputBufferAdcTime * SAMPLEFREQ,
				frameCount,
				(double)1/(double)SAMPLEFREQ);
		PI_DataObj * out = pContainer->getItem(PC_IDX_AUDIO_16BIT_MONO);

		out->setType( PS_IDX_AUDIO | PC_IDX_AUDIO_16BIT_MONO );

		if(out->allocatedLength() < frameCount)
		{
			out->setLength(out->allocatedLength());
			qWarning("Buffer too small! Need room for %lu frames", frameCount);
		}
		else
			out->setLength( frameCount );

		Q_INT16 * output = (Q_INT16*)out->dataPtr();
		Q_INT16 * source = (Q_INT16*)data;

		for( unsigned int i=0; i < out->length(); i++ )
		{
			output[i] = source[(i << 1) + 1];
		}

		m_pWaveBuffer->returnWriteObj( pContainer );
	}

	return paContinue;
}

extern "C" {
PLUGIN_EXPORT void LoadObject( PI_Unknown ** ppUnknown )
{
	PaWaveIn *obj = new PaWaveIn();
	obj->queryInterface( PIID_UNKNOWN, (void**)ppUnknown );
}
} //extern "C"
