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

#include "spectrumdata.h"

#include <math.h>

#include "config/node_macro.h"

const unsigned char SpectrumData::c_ubcolormap[] = {
   255,     0,     0,
   251,    89,    17,
   245,   240,    28,
   114,   237,    31,
    61,   207,   179,
    44,   143,   224,
    86,   129,   182,
   127,    86,   192,
   202,    66,   183,
   153,     0,   153
};

const unsigned char SpectrumData::c_ubspectrummap[] = {
	  0,	  0,	143,
	  0,	  0,	159,
	  0,	  0,	175,
	  0,	  0,	191,
	  0,	  0,	207,
	  0,	  0,	223,
	  0,	  0,	239,
	  0,	  0,	255,
	  0,	 16,	255,
	  0,	 32,	255,
	  0,	 48,	255,
	  0,	 64,	255,
	  0,	 80,	255,
	  0,	 96,	255,
	  0,	112,	255,
	  0,	128,	255,
	  0,	143,	255,
	  0,	159,	255,
	  0,	175,	255,
	  0,	191,	255,
	  0,	207,	255,
	  0,	223,	255,
	  0,	239,	255,
	  0,	255,	255,
	 16,	255,	239,
	 32,	255,	223,
	 48,	255,	207,
	 64,	255,	191,
	 80,	255,	175,
	 96,	255,	159,
	112,	255,	143,
	128,	255,	128,
	143,	255,	112,
	159,	255,	 96,
	175,	255,	 80,
	191,	255,	 64,
	207,	255,	 48,
	223,	255,	 32,
	239,	255,	 16,
	255,	255,	  0,
	255,	239,	  0,
	255,	223,	  0,
	255,	207,	  0,
	255,	191,	  0,
	255,	175,	  0,
	255,	159,	  0,
	255,	143,	  0,
	255,	128,	  0,
	255,	112,	  0,
	255,	 96,	  0,
	255,	 80,	  0,
	255,	 64,	  0,
	255,	 48,	  0,
	255,	 32,	  0,
	255,	 16,	  0,
	255,	  0,	  0,
	239,	  0,	  0,
	223,	  0,	  0,
	207,	  0,	  0,
	191,	  0,	  0,
	175,	  0,	  0,
	159,	  0,	  0,
	143,	  0,	  0,
	128,	  0,	  0
};

const GLint SpectrumData::c_iFAfogMode = GL_EXP;
const float SpectrumData::c_fFAfogDensity = 4;

SpectrumData::SpectrumData( ObjectNode * config )
: m_ddeltac(m_ConfigControl), m_Config(config)
{
	SIMPLEVALUENODE_DECLARE_AND_GET(double,t_length,m_Config);
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,fs,m_Config);
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,hop_size,m_Config);
	c_nframes = (unsigned int)(VALUENODE_GET_VALUE(t_length) * (double)VALUENODE_GET_VALUE(fs) / (double)VALUENODE_GET_VALUE(hop_size) + 5);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,maxpointsperframe,m_Config);
	c_nbufsize = VALUENODE_GET_VALUE(maxpointsperframe);
}

SpectrumData::~SpectrumData()
{
}

void SpectrumData::init()
{
	glGenBuffersARB( 1, &m_VBODataBuffer );
	glGenBuffersARB( 1, &m_VBOColor1Buffer );
	glGenBuffersARB( 1, &m_VBOColor2Buffer );

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBODataBuffer );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, c_nbufsize*c_nframes*sizeof(CVertex), NULL, GL_DYNAMIC_DRAW_ARB );

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor1Buffer );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, c_nbufsize*c_nframes*sizeof(CColor), NULL, GL_DYNAMIC_DRAW_ARB );

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor2Buffer );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, c_nbufsize*c_nframes*sizeof(CColor), NULL, GL_DYNAMIC_DRAW_ARB );

	m_BufferSizes = new int[c_nframes];
	memset(m_BufferSizes, 0, c_nframes*sizeof(*m_BufferSizes));
	m_curVB = 0;
}

void SpectrumData::cleanup()
{
	glDeleteBuffersARB( 1, &m_VBODataBuffer );
	glDeleteBuffersARB( 1, &m_VBOColor1Buffer );
	glDeleteBuffersARB( 1, &m_VBOColor2Buffer );
	delete[] m_BufferSizes;
}

void SpectrumData::addData(double *A, double *F, unsigned int length, double f0, double reftime)
{
	unsigned int count = 0;

	m_curVB = (m_curVB + 1) % c_nframes;

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBODataBuffer );
	CVertex * vbuffer = (CVertex *)glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor1Buffer );
	CColor * cbuffer = (CColor *)glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor2Buffer );
	CColor * sbuffer = (CColor *)glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );

	if( vbuffer != NULL && cbuffer != NULL && sbuffer != NULL )
	{
		vbuffer += m_curVB*c_nbufsize;
		cbuffer += m_curVB*c_nbufsize;
		sbuffer += m_curVB*c_nbufsize;
	}
	else
	{
		if( vbuffer != NULL )
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBODataBuffer );
			glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );
		}
		if( cbuffer != NULL )
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor1Buffer );
			glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );
		}
		if( sbuffer != NULL )
		{
			glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor2Buffer );
			glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );
		}
		m_BufferSizes[m_curVB] = 0;
		return;
	}

	int maplen = sizeof(c_ubcolormap)/sizeof(unsigned char)/3;
	double hfac = pow(1.0005777895065548592967925757932,m_ddeltac.getPrimaryBuffer() );
	double f0hlow = 1/hfac-1;
	double f0hhigh = hfac-1;
	float cfac = 1;
	for( unsigned int i = 0; i < length && i < c_nbufsize; i++ )
	{
		double FrelF0 = F[i]/f0;
		int bandind = (int)floor(FrelF0+0.5);
		FrelF0 = FrelF0 - bandind;
		int mapind = (bandind-1+maplen) % maplen;
		if( FrelF0 >= f0hlow && FrelF0 <= f0hhigh )
			cfac = 1;
		else
		{
			cfac = 0.5;
		}

		cbuffer[count].r = (GLubyte)(c_ubcolormap[3*mapind]*cfac);
		cbuffer[count].g = (GLubyte)(c_ubcolormap[3*mapind+1]*cfac);
		cbuffer[count].b = (GLubyte)(c_ubcolormap[3*mapind+2]*cfac);
		cbuffer[count].a = (GLubyte)(255*cfac);

		float Alimit;
		if(A[i] > 0.0)
			Alimit = 0.0;
		else if(A[i] < -70.0)
			Alimit = -70.0;
		else
			Alimit = A[i];

		float find = (Alimit+70)/(70)*(sizeof(c_ubspectrummap)/(3*sizeof(unsigned char))-1);
		int lind = (int)floor(find);
		int uind = (int)ceil(find);
		float bfac = find-lind;

		sbuffer[count].r = (GLubyte)(c_ubspectrummap[3*lind]*bfac+c_ubspectrummap[3*uind]*(1-bfac));
		sbuffer[count].g = (GLubyte)(c_ubspectrummap[3*lind+1]*bfac+c_ubspectrummap[3*uind+1]*(1-bfac));
		sbuffer[count].b = (GLubyte)(c_ubspectrummap[3*lind+2]*bfac+c_ubspectrummap[3*uind+2]*(1-bfac));
		sbuffer[count].a = 255;

		vbuffer[count].x = F[i];
		vbuffer[count].y = A[i];
		vbuffer[count].z = reftime;

		count++;
	}
	m_BufferSizes[m_curVB] = count; //// -1 ????

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBODataBuffer );
	if( !glUnmapBufferARB( GL_ARRAY_BUFFER_ARB ) )
	{
		m_BufferSizes[m_curVB] = 0;
	}
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor1Buffer );
	if( !glUnmapBufferARB( GL_ARRAY_BUFFER_ARB ) )
	{
		m_BufferSizes[m_curVB] = 0;
	}
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor2Buffer );
	if( !glUnmapBufferARB( GL_ARRAY_BUFFER_ARB ) )
	{
		m_BufferSizes[m_curVB] = 0;
	}
}

void SpectrumData::update()
{
	// Update configuration
	m_ConfigControl.update();
}

void SpectrumData::draw( const struct ViewState & vs, float offset )
{
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_VERTEX_ARRAY );

	switch( vs.eView )
	{
	case FA_Atlas:
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor1Buffer );
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, (CColor*)NULL);
		glEnable(GL_FOG);
		{
			glFogi (GL_FOG_MODE, c_iFAfogMode );
			glFogf (GL_FOG_DENSITY, c_fFAfogDensity );
			glHint (GL_FOG_HINT, GL_NICEST);
			glFogf (GL_FOG_START, 0);
			glFogf (GL_FOG_END, 1);//31
		}
		break;
	case TA_Atlas:
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor1Buffer );
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, (CColor*)NULL);
		break;
	case TF_Atlas:
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor1Buffer );
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, (CColor*)NULL);
		break;
	case TF_Spectrum:
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBOColor2Buffer );
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, (CColor*)NULL);
		// Use first color in map as background
		glColor3ubv( c_ubspectrummap );
		glBegin( GL_QUADS );
		{
			glVertex3f( vs.fHigh, vs.aLow+0.1, vs.tHigh );
			glVertex3f( vs.fHigh, vs.aLow+0.1, vs.tLow );
			glVertex3f( vs.fLow, vs.aLow+0.1, vs.tLow );
			glVertex3f( vs.fLow, vs.aLow+0.1, vs.tHigh );
		}
		glEnd();
		break;
	}

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBODataBuffer );
	glVertexPointer( 3, GL_FLOAT, 0, (CVertex*)NULL);

	glTranslatef( 0, 0, -offset );

	for( int i = 0; i < (int)c_nframes; i++ )
	{
		int curPos = (m_curVB - i + c_nframes) % c_nframes;

		if( m_BufferSizes[curPos] > 0 )
		{
			glDrawArrays( GL_POINTS, curPos*c_nbufsize, m_BufferSizes[curPos] );
		}
	}

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

	if(vs.eView == FA_Atlas)
		glDisable(GL_FOG);

	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );
}
