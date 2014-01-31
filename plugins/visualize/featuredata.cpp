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

#include "featuredata.h"

#ifdef Q_OS_WIN
#define _USE_MATH_DEFINES
#define IS_NAN(x) _isnan(x)
#include <float.h>
#endif
#ifdef Q_OS_LINUX
#define IS_NAN(x) isnan(x)
#endif
#include <math.h>

#include "config/node_macro.h"

FeatureData::FeatureData( ObjectNode * config )
: m_Config(config)
{
	SIMPLEVALUENODE_DECLARE_AND_GET(double,t_length,m_Config);
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,fs,m_Config);
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,hop_size,m_Config);
	c_nframes = (unsigned int)(VALUENODE_GET_VALUE(t_length) * (double)VALUENODE_GET_VALUE(fs) / (double)VALUENODE_GET_VALUE(hop_size) + 5);
}

FeatureData::~FeatureData()
{
}

void FeatureData::init()
{
	glGenBuffersARB(NUM_SHORTTIME_FEATURES,m_VBO);

	for(int i = 0; i < NUM_SHORTTIME_FEATURES; i++)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBO[i]);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,c_nframes*2*sizeof(GLfloat),NULL,GL_STREAM_DRAW_ARB);
	}
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

	m_DataIndex = 0;
}

void FeatureData::cleanup()
{
	glDeleteBuffersARB(NUM_SHORTTIME_FEATURES,m_VBO);
}

void FeatureData::addData(double *pD, unsigned int length, double reftime)
{
	(void) length;

	GLfloat vertex[2];

	for(int i = 0; i < NUM_SHORTTIME_FEATURES; i++)
	{
		if(IS_NAN(pD[i]))
			continue;

		vertex[0] = reftime;

		switch(i)
		{
		case PV_IDX_ANA_SF_FUND_FREQ:
		case PV_IDX_ANA_SF_FUND_FREQ_QUAL:
		case PV_IDX_ANA_SF_FULL_SPECTRAL_CENTROID:
		case PV_IDX_ANA_SF_HARMONIC_SLOPE:
		case PV_IDX_ANA_SF_LOCAL_MAXIMA_RATE:
			vertex[1] = pD[i];
			break;
		case PV_IDX_ANA_SF_RMS_AMP:
		case PV_IDX_ANA_SF_MAX_AMP:
		case PV_IDX_ANA_SF_HI_TO_ALL:
			vertex[1] = 20*log10(pD[i]);
			break;
		case PV_IDX_ANA_SF_PARTIAL_1:
		case PV_IDX_ANA_SF_PARTIAL_2:
		case PV_IDX_ANA_SF_PARTIAL_3:
			vertex[1] = 10*log10(pD[i]);
			break;
		case PV_IDX_ANA_SF_INTONATION:
			{
				double f = pD[i];
				int logfreq = (int)floor(log(f/m_RefTuning)*17.312340490667560888319096172023+0.5)+9; // 17.31... = 1/log(2^(1/12))
				int octave = (int)floor(logfreq / 12.0);
				int note = logfreq - 12 * octave;
				double reffreq = pow(2.0,(12*octave+note-9)/12.0)*m_RefTuning;
				int cent = (int)(log(f/reffreq)*1731.2340490667560888319096172023);
				vertex[1] = cent;
			}
			break;
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBO[i]);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,m_DataIndex*sizeof(vertex),sizeof(vertex),&vertex);
	}

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

	m_DataIndex = (m_DataIndex + 1) % c_nframes;
}

void FeatureData::update()
{
	// Update configuration
}

void FeatureData::draw( float offset, t_curves_mask c_mask )
{
	GLubyte colors[NUM_SHORTTIME_FEATURES][3] = { {  0, 255, 255},
												  {  0, 128, 255},
												  {  0, 255, 255},
												  { 50, 128, 255},
												  {  0, 255, 255},
												  {255,   0,  0,},
												  {251,  89,  17},
												  {245, 240,  28},
												  {  0, 255, 255},
												  {  0, 255, 255},
												  {  0, 255, 255},
												  {  0, 255, 255}};

	glTranslatef(-offset,0,0);

	glEnableClientState(GL_VERTEX_ARRAY);
	{
		for( int i = 0; i < NUM_SHORTTIME_FEATURES; i++)
		{
			if( c_mask.mask[i] == 0)
				continue;

			glBindBufferARB(GL_ARRAY_BUFFER_ARB,m_VBO[i]);
			glVertexPointer(2,GL_FLOAT,0,0);

			glColor3ubv(colors[i]);
			glDrawArrays(GL_POINTS,0,c_nframes);
		}
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}
