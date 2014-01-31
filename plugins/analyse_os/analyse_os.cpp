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

#include "analyse_os.h"
#include "dataobj.h"
#include "streamcontainer.h"
#include "fundfreq.h"

#include "config/node_macro.h"

#ifdef Q_OS_WIN
#define _USE_MATH_DEFINES
static const Q_UINT32 _nan[2]={0xffffffff, 0x7fffffff};
#define NAN (*(double*)_nan)
#define IS_NAN(x) _isnan(x)
#include <float.h>
#endif
#ifdef Q_OS_LINUX
#define IS_NAN(x) isnan(x)
#endif
#include <math.h>

#include <cblas.h>

Analyse_os::Analyse_os() :
	m_buffer(NULL),m_pCompoundBuffer(NULL), m_pWaveReader(NULL), m_pProvider(NULL),
			m_OutFrameRefCount(0), m_isrunning(false), m_refcount(0),
			m_refLock(TRUE)
{
	m_pFundFreq = NULL;
}

Analyse_os::~Analyse_os()
{
	stopRunning();

	if (m_pWaveReader)
		delete m_pWaveReader;
	if (m_pCompoundBuffer)
		delete m_pCompoundBuffer;
	if (m_pProvider)
		m_pProvider->release();
	if(m_buffer)
        fftw_free(m_buffer);
	if(m_pFundFreq)
		delete m_pFundFreq;
}

bool Analyse_os::configure(PluginNode * config)
{
	OBJECTNODE_DECLARE_AND_GET(Analyse_os,Analyse_os,config);
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,fs,NODE_VARIABLE_NAME(Analyse_os));
	c_fs = VALUENODE_GET_VALUE(fs);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,frame_size,NODE_VARIABLE_NAME(Analyse_os));
	c_nframe = VALUENODE_GET_VALUE(frame_size);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,hop_size,NODE_VARIABLE_NAME(Analyse_os));
	c_hop = VALUENODE_GET_VALUE(hop_size);

	//Ncenter = ceil(Nframe/2);
	c_ncenter = (int)ceil((double)c_nframe/2.0) - 1;
    //Nfft = Nframe*2-2;
	c_nfft = c_nframe * 2 - 2;
	//fmax = min(20000, 1000*floor(fs/2000));
	//double fmax = 20000.0 < 1000.0*floor(c_fs/2000.0) ? 20000.0 : 1000.0*floor(c_fs/2000.0);

	m_buffer = (double*)fftw_malloc(c_nframe*sizeof(double));
	m_diffbuffer = (double*)fftw_malloc(c_nframe*sizeof(double));
	m_win = (double*)fftw_malloc(c_nframe*sizeof(double));
	m_korr = (double*)fftw_malloc((c_nframe-1)*sizeof(double));
	m_Y_in = (double*)fftw_malloc((c_nfft)*sizeof(double));
	m_DY_in = (double*)fftw_malloc((c_nfft)*sizeof(double));
	m_Y = (fftw_complex*)fftw_malloc((c_nframe)*sizeof(fftw_complex));
	m_DY = (fftw_complex*)fftw_malloc((c_nframe)*sizeof(fftw_complex));
	m_YA = (double*)fftw_malloc((c_nframe-1)*sizeof(double));
	m_DYA = (double*)fftw_malloc((c_nframe-1)*sizeof(double));
	m_Ypb = (bool*)fftw_malloc((c_nframe-1)*sizeof(bool));
	m_A = (double*)fftw_malloc((c_nframe-1)*sizeof(double));

	OBJECTNODE_DECLARE_AND_GET(m_pFundFreq,FundFreq,config);
	m_pFundFreq = new FundFreq(NODE_VARIABLE_NAME(m_pFundFreq),c_nframe,c_nfft,c_fs);

	//% Define Gauss window-----------------------------------------------------%
	//w = 2/sum(gausswin(Nframe,4))*gausswin(Nframe,4);
	//win = w';
	double gw_r = 4;
	double gw_n = c_nframe;
	double gw_k;
	double gw_f = gw_r * 2.0 / gw_n;
	double gw_f2 = gw_f * gw_f;
	double gw_sum = 0;
	for(int i = 0; i < gw_n; i++)
	{
		gw_k = (double)i - ( gw_n - 1.0 ) / 2.0;
		m_win[i] = exp( - 0.5 * gw_f2 * gw_k * gw_k );
		gw_sum += m_win[i];
	}
	for(int i = 0; i < gw_n; i++)
	{
		m_win[i] = 2.0 / gw_sum * m_win[i];
	}

	//% Correction function-----------------------------------------------------%
	//k=[0:2*Nframe-3];
	//f=fs*k/(2*Nframe-3);
	//z=2*pi*f/fs; z(1) = eps;
	//korr=(z./(2*sin(z/2)));
	double kr_z;
	double kr_sin;
	m_korr[0] = 1;
	for(Q_UINT32 i = 1; i < c_nframe-1; i++)
	{
		kr_z = 2 * M_PI * (c_fs*i/(c_nfft-1))/c_fs;
		kr_sin = 2.0 * sin(kr_z/2.0);
		m_korr[i] = fabs(kr_z/kr_sin);
	}

	m_Y_plan = fftw_plan_dft_r2c_1d(c_nfft,m_Y_in,m_Y,FFTW_PATIENT | FFTW_DESTROY_INPUT);
	m_DY_plan = fftw_plan_dft_r2c_1d(c_nfft,m_DY_in,m_DY,FFTW_PATIENT | FFTW_DESTROY_INPUT);

	m_pCompoundBuffer
			= new DataBuffer(
					20,
					new StreamContainer(
							new DataObj< double, false >( c_nfft/2 +1, PC_STR_ANALYSIS_A, PT_STR_DOUBLE ),
							new DataObj< double, false >( c_nfft/2 +1, PC_STR_ANALYSIS_F, PT_STR_DOUBLE ),
							new DataObj< double, false >( NUM_SHORTTIME_FEATURES, PC_STR_ANALYSIS_SF, PT_STR_DOUBLE ),
							NULL
					)
			);
	return true;
}

bool Analyse_os::stopRunning()
{
	m_isrunning = false;
	wait();
	return true;
}

bool Analyse_os::startRunning()
{
	m_isrunning = true;
	start();
	return true;
}

void Analyse_os::run()
{
	StreamContainer * pInputBuffer= NULL;
	StreamContainer * pLastInputBuffer = NULL;

	Q_UINT32 ibPos = 0;
	Q_UINT32 ifPos = 0;

	while (m_isrunning)
	{
		if (pInputBuffer)
		{
			PI_DataObj * pDataBuffer = pInputBuffer->getItem(PC_IDX_AUDIO_16BIT_MONO);
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

void Analyse_os::process()
{
	StreamContainer * pContainer = m_pCompoundBuffer->getWriteObj(m_OutFrameRefCount,c_nframe,(double)1/(double)c_fs);
	PI_DataObj * pAmplitude = pContainer->getItem(PC_IDX_ANALYSIS_A);
	PI_DataObj * pFrequency	= pContainer->getItem(PC_IDX_ANALYSIS_F);
	PI_DataObj * pShortTime = pContainer->getItem(PC_IDX_ANALYSIS_SF);

	double * ampl = (double*)pAmplitude->dataPtr();
	double * freq = (double*)pFrequency->dataPtr();

	//% Compute derivative of signal--------------------------------------------%
	//y2 = y; Is not used
	//y = y(1:Nframe);
	//This is given
	//dy = fs*[0 diff(y)];
	for(Q_UINT32 i = 1; i < c_nframe; i++)
	{
		m_diffbuffer[i] = c_fs * ( m_buffer[i] - m_buffer[i-1] );
	}
	m_diffbuffer[0] = 0;

	//% Zero phase windowing----------------------------------------------------%
	//yw = y.*win; dy = dy.*win;
	//dy = [dy zeros(1,Nframe-2)];
	//yz = [yw(Ncenter:Nframe) zeros(1,Nframe-2) yw(1:Ncenter-1)];
	for(Q_UINT32 i = 0; i < c_nframe; i++)
	{
		m_DY_in[i] = m_win[i] * m_diffbuffer[i];
	}
	for(Q_UINT32 i = c_nframe; i < c_nfft; i++)
	{
		m_DY_in[i] = 0;
	}
	for(Q_UINT32 i = c_ncenter; i < c_nframe; i++)
	{
		m_Y_in[i-c_ncenter] = m_win[i] * m_buffer[i];
	}
	for(Q_UINT32 i = 0; i < c_ncenter; i++)
	{
		m_Y_in[i+(c_nframe-c_ncenter)+(c_nframe-2)] = m_win[i] * m_buffer[i];
	}
	for(Q_UINT32 i = 0; i < c_nframe-2; i++)
	{
		m_Y_in[i+c_nframe-c_ncenter] = 0;
	}


	//% Compute fft and apply correction to DY----------------------------------%
	//Y = fft(yz,Nfft);
	fftw_execute(m_Y_plan);
	//DY = korr.*fft(dy,Nfft);
	fftw_execute(m_DY_plan);

	//% Magnitude spectra-------------------------------------------------------%
	//YA = abs(Y(1:Nframe-1));
	//DYA = abs(DY(1:Nframe-1));
	for(Q_UINT32 i = 0; i < c_nframe-1; i++)
	{
		m_YA[i] = sqrt(m_Y[i][0]*m_Y[i][0]+m_Y[i][1]*m_Y[i][1]);
		m_DYA[i] = m_korr[i] * sqrt(m_DY[i][0]*m_DY[i][0]+m_DY[i][1]*m_DY[i][1]);
	}

	//% Find local maxima-------------------------------------------------------%
	//Yp = find((diff([-Inf,YA])>0)&(diff([YA -Inf])<0));
	m_Ypb[0] = 0;
	m_Ypb[c_nframe-2] = 0;
	Q_UINT32 peak_len = 0;
	for(Q_UINT32 i = 1; i < c_nframe-2; i++)
	{
		if(m_YA[i-1] < m_YA[i] && m_YA[i] > m_YA[i+1])
		{
			m_Ypb[i] = true;
			peak_len++;
		}
		else
		{
			m_Ypb[i] = false;
		}
	}

	//% Eliminate zero and max freq---------------------------------------------%
	//Yp(Yp == 1) = [];
	//Yp(Yp == length(YA)) = [];
	// Not necessary

	//% Frequency reassignment--------------------------------------------------%
	//F = 1/(2*pi)*DYA(Yp)./YA(Yp);
	int peak_i = 0;
	for(Q_UINT32 i = 0; i < c_nframe-1; i++)
	{
		if(m_Ypb[i])
		{
			double ya = m_YA[i];
			freq[peak_i] = 1.0/(2.0*M_PI) * m_DYA[i] / ya;
			ampl[peak_i] = 20.0 * log10(ya);
			m_A[peak_i] = ya;
			peak_i++;
		}
	}

	//% Magnitude correction (modified Brent algorithm)-------------------------%
	//Ypl = Yp-1;
	//Ypr = Yp+1;
	//cor = (log(YA(Ypl))-log(YA(Ypr)))./ (2*(log(YA(Ypl))-2*log(YA(Yp))+log(YA(Ypr))));
	//A = exp(log(YA(Yp))+cor.*(log(YA(Ypl))-log(YA(Ypr)))/4);
	// Not implemented

	//% Reduce to reasonable frequency range-- ---------------------------------%
	//A = A(F<=fmax);
	//F = F(F<=fmax);
	//%Yp = Yp(F<=fmax);
	// Not done, is probably not necessary

	pAmplitude->setLength(peak_len);
	pAmplitude->setType(PS_IDX_ANALYSIS | PC_IDX_ANALYSIS_A);
	pFrequency->setLength(peak_len);
	pFrequency->setType(PS_IDX_ANALYSIS | PC_IDX_ANALYSIS_F);

	double f0;
	double f0_p;
	m_pFundFreq->findF0( m_buffer, (double*)pFrequency->dataPtr(), m_A, peak_len, &f0, &f0_p );

	int y_max_i = cblas_idamax(c_nframe,m_buffer,1);
	double y_max = fabs(m_buffer[y_max_i]);

	double y_rms = cblas_dnrm2(c_nframe,m_buffer,1);
	y_rms = y_rms/sqrt((double)c_nframe);

	// local maxima rate
	// TODO: what about negative maximums?
	double num_lomax_y = 0;
	for(Q_UINT32 i = 1; i < c_nframe-1; i++)
	{
		if(m_buffer[i-1] < m_buffer[i] && m_buffer[i] > m_buffer[i+1])
			num_lomax_y++;
	}
	double lomax_rate = num_lomax_y/(c_nframe/2);

	// Full spectral centroid
	double spc = 0;
	spc = cblas_ddot(peak_len, freq, 1, m_A, 1) / cblas_dasum(peak_len, m_A, 1);

	// Harmonic frequencies and amplitudes
	unsigned int np = 20000.0 / f0;
	double Fharm[100];
	double Aharm[100];
	if(np > 100)
		np = 100;
	for(Q_UINT32 i = 0; i < np; i++)
	{
		double Fp = 0;
		double Apsum = 0;
		double Ap = 0;
		for(Q_UINT32 j = 0; j < peak_len; j++)
		{
			if(fabs((1200.0/log(2.0))*log(freq[j]/(f0 * (i+1)))) <= 40.0)
			{
				Fp += freq[j] * m_A[j];
				Apsum += m_A[j];
				if(Ap < m_A[j])
					Ap = m_A[j];
			}
		}
		if(Ap > 0)
		{
			Fp = Fp/Apsum;
			Fharm[i] = Fp;
			Aharm[i] = Ap;
		}
		else
		{
			Fharm[i] = NAN;
			Aharm[i] = 0;
		}
	}

	// Partials 1, 2, 3
	double a1, a2, a3;
	if(np >= 3)
		a3 = Aharm[2] * Aharm[2];
	if(np >= 2)
		a2 = Aharm[1] * Aharm[1];
	a1 = Aharm[0] * Aharm[0];

	// hitoall
	double hitoall = 0;
	hitoall = cblas_dasum(np-1,&Aharm[1],1);
	hitoall = hitoall / (hitoall + Aharm[0]);

	// Harmonic shape parameters
	unsigned int hamlen = 0;
	double Fnorm[100];
	double Anorm[100];
	double Amax = 0;
	Amax = Aharm[cblas_idamax(np, Aharm, 1)];
	for(unsigned int i = 0; i < np; i++)
	{
		if(!IS_NAN(Fharm[i]))
		{
			Fnorm[hamlen] = log(Fharm[i] / f0);
			Anorm[hamlen] = log(Aharm[i] / Amax);
			hamlen++;
		}
	}

	double M[4] = {0,0,0,0};
	double Minv[4];
	double B[2] = {0,0};
	double u[2];
	double lambda = 1;
	double hsl;
	double hco;
	double Areg[100];
	if(hamlen >= 2)
	{
		for(unsigned int i = 0; i < hamlen; i++)
		{
			double temp1, temp2;

			temp1 = exp(lambda * Anorm[i]);
			temp2 = temp1 * Anorm[i];
			M[3] += temp1;
			temp1 *= Fnorm[i];
			M[1] += temp1;
			temp1 *= Fnorm[i];
			M[0] += temp1;
			B[0] += temp2;
			temp2 *= Fnorm[i];
			B[1] += temp2;
		}
		M[2] = M[0];

		double det = 1/(M[0]*M[3]+M[1]*M[2]);
		Minv[0] = det * M[3];
		Minv[1] = det * (-M[1]);
		Minv[2] = det * (-M[2]);
		Minv[3] = det * M[0];

		u[0] = Minv[0] * B[0] + Minv[1] * B[1];
		u[1] = Minv[2] * B[0] + Minv[3] * B[1];

		hsl = u[0];

		for(unsigned int i = 0; i < hamlen; i++)
		{
			Areg[i] = u[0] * Fnorm[i] + u[1];
		}

		// Calculate dot product, and norm
		double dotprod = cblas_ddot(hamlen,Anorm,1,Areg,1);
		double leftnorm = cblas_dnrm2(hamlen,Anorm,1);
		double rightnorm = cblas_dnrm2(hamlen,Areg,1);

		hco = dotprod / ( leftnorm * rightnorm );
		hco = hco * hco;
	}
	else
	{
		hsl = NAN;
		hco = NAN;
	}


	double * st = (double*)pShortTime->dataPtr();
	pShortTime->setLength(NUM_SHORTTIME_FEATURES);
	pShortTime->setType(PS_IDX_ANALYSIS | PC_IDX_ANALYSIS_SF);
	st[PV_IDX_ANA_SF_FUND_FREQ] = f0;
	st[PV_IDX_ANA_SF_FUND_FREQ_QUAL] = f0_p;
	st[PV_IDX_ANA_SF_MAX_AMP] = y_max;
	st[PV_IDX_ANA_SF_RMS_AMP] = y_rms;
	st[PV_IDX_ANA_SF_LOCAL_MAXIMA_RATE] = lomax_rate;
	st[PV_IDX_ANA_SF_FULL_SPECTRAL_CENTROID] = spc;
	st[PV_IDX_ANA_SF_PARTIAL_1] = a1;
	st[PV_IDX_ANA_SF_PARTIAL_2] = a2;
	st[PV_IDX_ANA_SF_PARTIAL_3] = a3;
	st[PV_IDX_ANA_SF_HI_TO_ALL] = hitoall;
	st[PV_IDX_ANA_SF_HARMONIC_SLOPE] = hsl;
	st[PV_IDX_ANA_SF_INTONATION] = f0;

	m_pCompoundBuffer->returnWriteObj(pContainer);
}


extern "C"
{
PLUGIN_EXPORT void LoadObject( PI_Unknown ** ppUnknown )
{
	Analyse_os *obj = new Analyse_os();
	obj->queryInterface( PIID_UNKNOWN, (void**)ppUnknown );
}
} //extern "C"
