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

#ifndef FUNDFREQ_H
#define FUNDFREQ_H

#include <qfile.h>
#include <qstring.h>
#include <qdatastream.h>

class ObjectNode;

class FundFreq
{
public:
	FundFreq( ObjectNode * config, int nframe, int nfft, int fs );
	virtual ~FundFreq();
	virtual void findF0( double * pFrame, double * pF, double * pA, int len, double * f0, double * reability );

protected:
	virtual double estimate( double * pF, double * pA, int len );
	virtual void correct( double * pFrame, double f0_e, double * f0_raw, double * f0_exact, double * f0_p );

private:
	class IndCompare
	{
	public:
		IndCompare( double * pData ):m_pData(pData) {}
		bool operator()( unsigned short arg1, unsigned short arg2 ) { return m_pData[arg1] > m_pData[arg2]; }
	private:
		double * m_pData;
	};

	class F0History
	{
	public:
		F0History( int histlen );
		~F0History();
		void insert( double value );
		void insert_nan();
		double median();

	private:
		struct listobj
		{
			listobj( double val, bool valid = true )
				:hprev(NULL),hnext(NULL),sprev(NULL),snext(NULL),isvalid(valid),value(val)
			{
			}
			listobj * hprev;
			listobj * hnext;
			listobj * sprev;
			listobj * snext;
			bool isvalid;
			double value;
		};

		listobj * takelastout();

		int slength;
		listobj * sortedlisthead;
		listobj * historylisthead;
		listobj * historylisttail;
	};

	const int c_nframe;
	const int c_nfft;
	const int c_Fs;

	int c_range;
	double c_d;
	int c_histlen;
	int c_considerlen;
	int c_nstrongest;

	F0History * m_pHistory;

	double * m_pRho;

	double * m_pFVector;
	double * m_pAVector;
	unsigned short * m_pSortInd;
};

#endif // FUNDFREQ_H
