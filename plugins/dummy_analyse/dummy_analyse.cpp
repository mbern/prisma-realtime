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

#include "dummy_analyse.h"
#include "dataobj.h"
#include "streamcontainer.h"

#ifdef Q_OS_WIN
#define _USE_MATH_DEFINES
#endif
#include <math.h>

const Q_UINT32 DummyAnalyse::c_fs = 44100;
const Q_UINT32 DummyAnalyse::c_fftorder = 10;
const Q_UINT32 DummyAnalyse::c_nfft = 1 << c_fftorder;
const Q_UINT32 DummyAnalyse::c_ncenter = 513;
const Q_UINT32 DummyAnalyse::c_hop = 768;
const Q_UINT32 DummyAnalyse::c_nframe = 1025;

DummyAnalyse::DummyAnalyse() :
	m_pCompoundBuffer(NULL), m_pWaveReader(NULL), m_pProvider(NULL),
			m_OutFrameRefCount(0), m_isrunning(false), m_refcount(0),
			m_refLock(TRUE)
{
	m_buffer = new double[c_nframe];
	m_bfft = new double[c_nfft + 1];
	m_window = new double[c_nfft];
	m_ampl = new double[c_nfft];
	for(Q_UINT32 i = 0; i < c_nfft; i++)
		m_window[i] = sin((double)i/(c_nfft-1.0)*2.0*M_PI + 1.5*M_PI)/2.0 + 0.5;
}

DummyAnalyse::~DummyAnalyse()
{
	stopRunning();

	if (m_pWaveReader)
		delete m_pWaveReader;
	if (m_pCompoundBuffer)
		delete m_pCompoundBuffer;
	if (m_pProvider)
		m_pProvider->release();

	delete[] m_buffer;
	delete[] m_bfft;
	delete[] m_window;
	delete[] m_ampl;
}

bool DummyAnalyse::configure(PluginNode * config)
{
	(void) config; //surpress warning

	m_pCompoundBuffer
			= new DataBuffer(
					20,
					new StreamContainer(
							new DataObj< double, false >( c_nfft/2 +1, "Corrected Amplitude", "double" ),
							new DataObj< double, false >( c_nfft/2 +1, "Corrected Frequency", "double" ),
							new DataObj< double, false >( c_nfft/2 +1, "Phases", "double" ),
							new DataObj< unsigned char, true >( c_nfft/2 +1, "Harmonic, Dominant", "unsigned char" ),
							new DataObj< double, false >( 10, "Fundamental, RMS", "double" ),
							NULL
					)
			);
	return true;
}

bool DummyAnalyse::stopRunning()
{
	m_isrunning = false;
	wait();
	return true;
}

bool DummyAnalyse::startRunning()
{
	m_isrunning = true;
	start();
	return true;
}

void DummyAnalyse::run()
{
	StreamContainer * pInputBuffer= NULL;
	StreamContainer * pLastInputBuffer = NULL;

	Q_UINT32 ibPos = 0;
	Q_UINT32 ifPos = 0;

	while (m_isrunning)
	{
		if (pInputBuffer)
		{
			PI_DataObj * pDataBuffer = pInputBuffer->getItem(0);
			Q_INT16 * inputdata = (Q_INT16*)pDataBuffer->dataPtr();

			Q_UINT32 iblen = pDataBuffer->length() - ibPos;
			Q_UINT32 iflen = c_nframe - ifPos;

			// Reached end of m_pInputBuffer ?
			if( iblen == 0 )
			{
				// Return old buffer and try to get the next
				pLastInputBuffer = pInputBuffer;
				pInputBuffer = m_pWaveReader->getNextObj();
				if( pInputBuffer != NULL )
				{
					delete pLastInputBuffer;
					pLastInputBuffer = NULL;
				}
				ibPos = 0;
			}
			// Is m_pInputFramed full ?
			else if( iflen == 0 )
			{
				// Shift data in m_pInputFramed
				for(Q_UINT32 i = 0; i < c_nframe - c_hop; i++)
					m_buffer[i] = m_buffer[i+c_hop];
				ifPos = c_nframe - c_hop;
			}
			// Is enough data in m_pInputBuffer to fill m_pInputFramed ?
			else if( iblen >= iflen )
			{
				// Fill m_pInputFramed
				for(Q_UINT32 i = 0; i < iflen; i++)
					m_buffer[i+ifPos] = (double)inputdata[i+ibPos] / ((double)(1 << 15));
				ibPos += iflen;
				ifPos += iflen;
				// m_pInputFramed full, calculate next step
				m_OutFrameRefCount = pInputBuffer->getRefNumberStart()+ibPos-c_nframe;
				process();
			}
			else
			{
				// Fill m_pInputFramed partly
				for(Q_UINT32 i = 0; i < iblen; i++)
					m_buffer[i+ifPos] = (double)inputdata[i+ibPos] / ((double)(1 << 15));
				ibPos += iblen;
				ifPos += iblen;
			}
		}
		else
		{
			msleep(1);
			if (pLastInputBuffer != NULL)
			{
				pInputBuffer = m_pWaveReader->getNextObj();
				if (pInputBuffer != NULL)
				{
					delete pLastInputBuffer;
					pLastInputBuffer = NULL;
				}
			}
			else
			{
				pInputBuffer = m_pWaveReader->getNewestObj();
			}
		}
	}
	// Return buffer in use
	if (pLastInputBuffer != NULL)
		delete pLastInputBuffer;
	if (pInputBuffer != NULL)
		delete pInputBuffer;
}

void DummyAnalyse::process()
{
	StreamContainer * pContainer = m_pCompoundBuffer->getWriteObj(m_OutFrameRefCount,c_nframe,(double)1/(double)c_fs);
	PI_DataObj * pAmplitude = pContainer->getItem(0);
	PI_DataObj * pFrequency	= pContainer->getItem(1);
	PI_DataObj * pPhase = pContainer->getItem(2);
	PI_DataObj * pSpectralProperty = pContainer->getItem(3);
	PI_DataObj * pShortTime = pContainer->getItem(4);

	for(Q_UINT32 i = 0; i < c_nfft; i++)
		m_bfft[i+1] = m_buffer[i]*m_window[i];

	rfft(m_bfft,c_nfft,c_fftorder);

	for(Q_UINT32 i = 0; i <= c_nfft/2; i++)
	{
		if(i == 0 || i == c_nfft/2)
		{
			m_ampl[i] = log( m_bfft[i+1] / ((double)c_nfft) );
		}
		else
		{
			m_ampl[i] = log( sqrt( m_bfft[i+1] * m_bfft[i+1] + m_bfft[(c_nfft-i)+1] * m_bfft[(c_nfft-i)+1] ) / ((double)c_nfft) );
		}
	}


	double * a = (double*)pAmplitude->dataPtr();
	double * f = (double*)pFrequency->dataPtr();
	unsigned char * sp = (unsigned char*)pSpectralProperty->dataPtr();

	pShortTime->setLength(4);
	double * st = (double*)pShortTime->dataPtr();

	Q_UINT32 len = 0;
	double amax = -1000;
	double fmax = 0;
	for(Q_UINT32 i = 1; i < c_nfft/2; i++)
	{
		if(m_ampl[i-1] < m_ampl[i] && m_ampl[i] > m_ampl[i+1])
		{
			a[len] = m_ampl[i];
			f[len] = (double)i * (double)c_fs/(double)c_nfft;
			sp[len] = 0;
			if(amax < a[len])
			{
				amax = a[len];
				fmax = f[len];
			}

			len++;
		}
	}

	pAmplitude->setLength(len);
	pFrequency->setLength(len);
	pPhase->setLength(len);
	pSpectralProperty->setLength(len);

	st[0] = fmax;
	st[1] = amax;

	st[2] = 0;
	st[3] = 0;
	for(Q_UINT32 i = 0; i < c_nframe; i++)
	{
		double v = fabs(m_buffer[i]);
		if(st[2] < v)
			st[2] = v;

		st[3] += m_buffer[i] * m_buffer[i];
	}
	st[3] = sqrt(st[3]/c_nframe);

	m_pCompoundBuffer->returnWriteObj(pContainer);
}

/****************************************************************************
* rfft(float X[],int N,int M)                                               *
*     A real-valued, in-place, split-radix FFT program                      *
*     Decimation-in-time, cos/sin in second loop                            *
*     Input: float X[1]...X[N] (NB Fortran style: 1st pt X[1] not X[0]!)    *
*     Length is N=2**M (i.e. N must be power of 2--no error checking)       *
*     Output in X[1]...X[N], in order:                                      *
*           [Re(0), Re(1),..., Re(N/2), Im(N/2-1),..., Im(1)]               *
*                                                                           *
* Original Fortran code by Sorensen; published in H.V. Sorensen, D.L. Jones,*
* M.T. Heideman, C.S. Burrus (1987) Real-valued fast fourier transform      *
* algorithms.  IEEE Trans on Acoustics, Speech, & Signal Processing, 35,    *
* 849-863.  Adapted to C by Bill Simpson, 1995  wsimpson@uwinnipeg.ca       *
****************************************************************************/

void DummyAnalyse::rfft(double X[],int N,int M)
{
	int I,I0,I1,I2,I3,I4,I5,I6,I7,I8, IS,ID;
	int J,K,N2,N4,N8;
	double A,A3,CC1,SS1,CC3,SS3,E,R1,XT;
	double T1,T2,T3,T4,T5,T6;

	/* ----Digit reverse counter--------------------------------------------- */
	J = 1;
	for(I=1;I<N;I++)
	{
		if (I<J)
		{
			XT    = X[J];
			X[J]  = X[I];
			X[I]  = XT;
		}
		K = N/2;
		while(K<J)
		{
			J -= K;
			K /= 2;
		}
		J += K;
	}

	/* ----Length two butterflies--------------------------------------------- */
	IS = 1;
	ID = 4;
	do
	{
		for(I0 = IS;I0<=N;I0+=ID)
		{
			I1    = I0 + 1;
			R1    = X[I0];
			X[I0] = R1 + X[I1];
			X[I1] = R1 - X[I1];
		}
		IS = 2 * ID - 1;
		ID = 4 * ID;
	}while(IS<N);
	/* ----L shaped butterflies----------------------------------------------- */
	N2 = 2;
	for(K=2;K<=M;K++)
	{
		N2    = N2 * 2;
		N4    = N2/4;
		N8    = N2/8;
		E     = (double) 2.0*M_PI/N2;
		IS    = 0;
		ID    = N2 * 2;
		do
		{
			for(I=IS;I<N;I+=ID)
			{
				I1 = I + 1;
				I2 = I1 + N4;
				I3 = I2 + N4;
				I4 = I3 + N4;
				T1 = X[I4] +X[I3];
				X[I4] = X[I4] - X[I3];
				X[I3] = X[I1] - T1;
				X[I1] = X[I1] + T1;
				if(N4!=1)
				{
					I1 += N8;
					I2 += N8;
					I3 += N8;
					I4 += N8;
					T1 = (X[I3] + X[I4])*M_SQRT1_2;
					T2 = (X[I3] - X[I4])*M_SQRT1_2;
					X[I4] = X[I2] - T1;
					X[I3] = -X[I2] - T1;
					X[I2] = X[I1] - T2;
					X[I1] = X[I1] + T2;
				}
			}
			IS = 2 * ID - N2;
			ID = 4 * ID;
		}while(IS<N);
		A = E;
		for(J= 2;J<=N8;J++)
		{
			A3 = 3.0 * A;
			CC1   = cos(A);
			SS1   = sin(A);  /*typo A3--really A?*/
			CC3   = cos(A3); /*typo 3--really A3?*/
			SS3   = sin(A3);
			A = (float)J * E;
			IS = 0;
			ID = 2 * N2;
			do
			{
				for(I=IS;I<N;I+=ID)
				{
					I1 = I + J;
					I2 = I1 + N4;
					I3 = I2 + N4;
					I4 = I3 + N4;
					I5 = I + N4 - J + 2;
					I6 = I5 + N4;
					I7 = I6 + N4;
					I8 = I7 + N4;
					T1 = X[I3] * CC1 + X[I7] * SS1;
					T2 = X[I7] * CC1 - X[I3] * SS1;
					T3 = X[I4] * CC3 + X[I8] * SS3;
					T4 = X[I8] * CC3 - X[I4] * SS3;
					T5 = T1 + T3;
					T6 = T2 + T4;
					T3 = T1 - T3;
					T4 = T2 - T4;
					T2 = X[I6] + T6;
					X[I3] = T6 - X[I6];
					X[I8] = T2;
					T2    = X[I2] - T3;
					X[I7] = -X[I2] - T3;
					X[I4] = T2;
					T1    = X[I1] + T5;
					X[I6] = X[I1] - T5;
					X[I1] = T1;
					T1    = X[I5] + T4;
					X[I5] = X[I5] - T4;
					X[I2] = T1;
				}
				IS = 2 * ID - N2;
				ID = 4 * ID;
			}while(IS<N);
		}
	}
	return;
}

extern "C"
{
PLUGIN_EXPORT void LoadObject( PI_Unknown ** ppUnknown )
{
	DummyAnalyse *obj = new DummyAnalyse();
	obj->queryInterface( PIID_UNKNOWN, (void**)ppUnknown );
}
} //extern "C"
