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

#include "filewriter.h"

#include <qdatastream.h>
#include <qfile.h>

#include "streamcontainer.h"

#include "config/node_macro.h"

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 1
#endif // WAVE_FORMAT_PCM

typedef struct {
	Q_UINT8		RIFF[4];
	Q_UINT32	ChunkSize;		// 32 Bit
	Q_UINT8		WAVEfmt[8];
	Q_UINT32	SubChunk1Size;
	Q_UINT16	AudioFormat;
	Q_UINT16	NumChannels;
	Q_UINT32	SampleRate;
	Q_UINT32	ByteRate;
	Q_UINT16	BlockAlign;
	Q_UINT16	BitsPerSample;
	Q_UINT8		data[4];
	Q_UINT32	SubChunk2Size;
} WavefileHeaderType;

FileWriter::FileWriter()
: m_refcount(0), m_refLock(TRUE),
  m_pTimeProvider(NULL), m_pDataBufferWaveProvider(NULL),
  m_pDataBufferCompoundProvider(NULL), m_FileWriteControl("SpectralDataSaveGui")
{
	m_pCompoundReader = NULL;
	m_pWaveReader = NULL;

	m_pCompoundTail = NULL;
	m_pWaveTail = NULL;

	m_TailCaptured = false;
	m_TailSynchronized = false;
}

FileWriter::~FileWriter()
{
	if(m_pCompoundTail != NULL)
		delete m_pCompoundTail;
	if(m_pWaveTail != NULL)
		delete m_pWaveTail;
	if(m_pCompoundReader != NULL)
		delete m_pCompoundReader;
	if(m_pWaveReader != NULL)
		delete m_pWaveReader;
	if(m_pTimeProvider != NULL)
		m_pTimeProvider->release();
	if(m_pDataBufferCompoundProvider != NULL)
		m_pDataBufferCompoundProvider->release();
	if(m_pDataBufferWaveProvider != NULL)
		m_pDataBufferWaveProvider->release();
}

void FileWriter::saveWaveBuffer( QString filename )
{
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Truncate );
	m_WaveBuffer.open( QIODevice::ReadOnly );

	WavefileHeaderType Header;

	memcpy(Header.RIFF,"RIFF",4);
	Header.ChunkSize		= m_WaveBuffer.size() + 36;
	memcpy(Header.WAVEfmt,"WAVEfmt ",8);
	Header.SubChunk1Size	= 16;
	Header.AudioFormat		= WAVE_FORMAT_PCM;
	Header.NumChannels		= 1;
	Header.SampleRate		= 44100;
	Header.ByteRate			= 44100 * 2;
	Header.BitsPerSample	= 16;
	Header.BlockAlign		= 2;
	memcpy(Header.data,"data",4);
	Header.SubChunk2Size	= m_WaveBuffer.size();

	file.writeBlock((const char *)&Header,sizeof(Header));

	char tempbuffer[1000];
	int readlength;
	while( true )
	{
		readlength = m_WaveBuffer.readBlock( tempbuffer, 1000 );
		if( readlength > 0 )
		{
			file.writeBlock( tempbuffer, readlength );
		}
		else
			break;
	}

	m_WaveBuffer.close();
	file.close();
}

void FileWriter::saveDataBuffer( QString filename )
{
	QFile file( filename );
	file.open( QIODevice::WriteOnly | QIODevice::Truncate );
	m_DataBuffer.open( QIODevice::ReadOnly );

	char tempbuffer[1000];
	int readlength;
	while( true )
	{
		readlength = m_DataBuffer.readBlock( tempbuffer, 1000 );
		if( readlength > 0 )
		{
			file.writeBlock( tempbuffer, readlength );
		}
		else
			break;
	}


	m_DataBuffer.close();
	file.close();
}

void FileWriter::copyBuffer()
{
	uint i = 0;
	Q_UINT32 sf_len;
	Q_UINT32 af_len;

	m_DataBuffer.open( QIODevice::WriteOnly | QIODevice::Truncate );
	m_WaveBuffer.open( QIODevice::WriteOnly | QIODevice::Truncate );
	QDataStream datastream( &m_DataBuffer );
	datastream.setByteOrder( QDataStream::LittleEndian );
	QDataStream wavestream( &m_WaveBuffer );
	wavestream.setByteOrder( QDataStream::LittleEndian );


	// Receive the current timestamp
	double currenttime = m_pTimeProvider->getTime();

	// Inform GUI
	emit bufferFilling( (int)(( currenttime - m_pCompoundTail->getRefTimeStart() ) / (double)m_BufferLength*100) );

	// Add a header and allow to distinguish the old format. The first two bytes are 0xFFFF which for the old
	// format indicates an invalid length. Afther this field there is a four byte version field
	datastream << (Q_UINT16)0xFFFF;
	datastream << (Q_UINT32)PRISMA_BIN_VERSION;

	// Copy compound buffer
	while(m_pCompoundTail->getRefTimeEnd() <= currenttime)
	{
		// Extract data
		PI_DataObj * amp = m_pCompoundTail->getItem(PC_IDX_ANALYSIS_A);
		PI_DataObj * freq = m_pCompoundTail->getItem(PC_IDX_ANALYSIS_F);
		PI_DataObj * sf = m_pCompoundTail->getItem(PC_IDX_ANALYSIS_SF);

		af_len = amp->length();
		sf_len = sf->length();

		// Put overall bytes for this frame (without this length field)
		datastream << (Q_UINT32)(2*af_len*sizeof(double)+sf_len*sizeof(double)+2*sizeof(Q_UINT32));

		// Copy and format data
		datastream << sf_len;
		for( i = 0; i < sf_len; i++)
		{
            datastream << *((double*)sf->dataPtr()+i);
		}

		datastream << af_len;
		for( i = 0; i < af_len; i++ )
		{
			datastream << *((double*)freq->dataPtr()+i);
		}
		for( i = 0; i < af_len; i++ )
		{
			datastream << *((double*)amp->dataPtr()+i);
		}

		// Get next object
		StreamContainer * tempobj = m_pCompoundTail;
		m_pCompoundTail = m_pCompoundReader->getNextObj();
		if(m_pCompoundTail != NULL)
		{
            delete tempobj;
		}
		else
		{
			m_pCompoundTail = tempobj;
			break;
		}
	}

	// Copy wave buffer
	while(m_pWaveTail->getRefTimeEnd() <= currenttime)
	{
		// Extract data
		PI_DataObj * wave = m_pWaveTail->getItem(PC_IDX_AUDIO_16BIT_MONO);

		// Copy and format data
		for( i = 0; i < wave->length(); i++ )
		{
			wavestream << *((Q_INT16*)wave->dataPtr()+i);
		}

		// Get next object
		StreamContainer * tempobj = m_pWaveTail;
		m_pWaveTail = m_pWaveReader->getNextObj();
		if(m_pWaveTail != NULL)
		{
            delete tempobj;
		}
		else
		{
			m_pWaveTail = tempobj;
			break;
		}
	}

	m_DataBuffer.close();
	m_WaveBuffer.close();

	//Release the tails
	delete m_pCompoundTail;
	m_pCompoundTail = NULL;
	delete m_pWaveTail;
	m_pWaveTail = NULL;
	m_TailCaptured = false;
	m_TailSynchronized = false;
}

void FileWriter::setBufferLength( int length )
{
	m_BufferLength = (double)length;
}

bool FileWriter::configure(PluginNode * config)
{
	(void) config; //surpress warning

	if( m_FileWriteControl.parent == NULL )
		return false;

	OBJECTNODE_DECLARE_AND_GET(m_FileWriteControl,FileWriteControl,config);
	m_FileWriteControl.widget = new FileWriteControl( NODE_VARIABLE_NAME(m_FileWriteControl), m_FileWriteControl.parent );
	m_FileWriteControl.widget->connect( this, SIGNAL(bufferFilling(int)), SLOT(setBufferFilling(int)) );
	this->connect( m_FileWriteControl.widget, SIGNAL(sendUpdate()), SLOT(updateBuffer()) );
	this->connect( m_FileWriteControl.widget, SIGNAL(sendLength(int)), SLOT(setBufferLength(int)) );
	this->connect( m_FileWriteControl.widget, SIGNAL(sendDataSave(QString)), SLOT(saveDataBuffer(QString)) );
	this->connect( m_FileWriteControl.widget, SIGNAL(sendWaveSave(QString)), SLOT(saveWaveBuffer(QString)) );
	this->connect( m_FileWriteControl.widget, SIGNAL(sendCopy()), SLOT(copyBuffer()) );

	emit bufferFilling( 0 );

	m_FileWriteControl.widget->fireConfiguration();

	return true;
}

bool FileWriter::stopRunning()
{
	m_FileWriteControl.widget->killTimer(timer_id);
	return true;
}

bool FileWriter::startRunning()
{
	timer_id = m_FileWriteControl.widget->startTimer(100);

	if(timer_id != 0)
		return true;
	else
		return false;
}

void FileWriter::updateBuffer()
{
	if( m_TailCaptured && m_TailSynchronized )
	{
		// Receive the current timestamp
		double currenttime = m_pTimeProvider->getTime();
		// First adjust the compound tail to the buffer size
		while( (currenttime - m_pCompoundTail->getRefTimeEnd()) > m_BufferLength )
		{
			StreamContainer * tempobj = m_pCompoundTail;
			m_pCompoundTail = m_pCompoundReader->getNextObj();
			if(m_pCompoundTail != NULL)
			{
                delete tempobj;
			}
			else
			{
				m_pCompoundTail = tempobj;
				break;
			}
		}
		// Next adjust the wave tail according to the compound tail
		while(m_pCompoundTail->getRefNumberStart() > m_pWaveTail->getRefNumberEnd())
		{
			StreamContainer * tempobj = m_pWaveTail;
			m_pWaveTail = m_pWaveReader->getNextObj();
			if(m_pWaveTail != NULL)
			{
				delete tempobj;
			}
			else
			{
				m_pWaveTail = tempobj;
				break;
			}
		}

		//Inform GUI
		emit bufferFilling( (int)(( currenttime - m_pCompoundTail->getRefTimeStart() ) / (double)m_BufferLength*100) );
	}
	else
	{
		if( m_TailCaptured )
		{
			// Try to synchronize the two streams
			if(m_pCompoundTail->getRefNumberStart() < m_pWaveTail->getRefNumberStart())
			{
				// Compound tail is older
				while(m_pCompoundTail->getRefNumberStart() < m_pWaveTail->getRefNumberStart())
				{
					StreamContainer * tempobj = m_pCompoundTail;
					m_pCompoundTail = m_pCompoundReader->getNextObj();
					if(m_pCompoundTail != NULL)
					{
						delete tempobj;
					}
					else
					{
						m_pCompoundTail = tempobj;
						break;
					}
				}
				if(m_pCompoundTail->getRefNumberStart() >= m_pWaveTail->getRefNumberStart())
				{
					// Compound tail starts in the range of wave tail, we are synchronized
					m_TailSynchronized = true;
				}
			}
			else if(m_pCompoundTail->getRefNumberStart() > m_pWaveTail->getRefNumberStart())
			{
				// Wave tail is older
				while(m_pCompoundTail->getRefNumberStart() > m_pWaveTail->getRefNumberEnd())
				{
					StreamContainer * tempobj = m_pWaveTail;
					m_pWaveTail = m_pWaveReader->getNextObj();
					if(m_pWaveTail != NULL)
					{
						delete tempobj;
					}
					else
					{
						m_pWaveTail = tempobj;
						break;
					}
				}
				if(m_pCompoundTail->getRefNumberStart() <= m_pWaveTail->getRefNumberEnd())
				{
					// Compound tail starts in the range of wave tail, we are synchronized
					m_TailSynchronized = true;
				}
			}
			else
			{
				// Same timestamp! We are already synchronized
				m_TailSynchronized = true;
			}

		}
		else
		{
			// Try to capture the tail when not captured already
			if(m_pCompoundTail == NULL)
				m_pCompoundTail = m_pCompoundReader->getNewestObj();
			if(m_pWaveTail == NULL)
				m_pWaveTail = m_pWaveReader->getNewestObj();

			// Change to captured state if both tails are captrued
			if(m_pCompoundTail != NULL && m_pWaveTail != NULL)
			{
				m_TailCaptured = true;
			}
		}
	}
}

void FileWriter::connectSlot( const QString & signal, const QObject * obj, const QString & slot )
{
	QString slot_str = SLOT(mySlot);
	QString slot_fcn = slot;

	if( signal == "bufferFilling" )
	{
		slot_fcn.append( "(int)" );
		slot_str.replace( "mySlot", slot_fcn );
		connect( this, SIGNAL(bufferFilling(int)), obj, slot_str );
	}
}

extern "C" {
PLUGIN_EXPORT void LoadObject( PI_Unknown ** ppUnknown )
{
	FileWriter *obj = new FileWriter();
	obj->queryInterface( PIID_UNKNOWN, (void**)ppUnknown );
}
} //extern "C"
