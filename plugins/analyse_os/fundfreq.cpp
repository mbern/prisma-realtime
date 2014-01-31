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

#include "fundfreq.h"

#include <math.h>
#include <stdlib.h>

#include <algorithm>

#include <qstring.h>

#include <cblas.h>

#include "config/node_macro.h"

static const double c_fac_one_tone = 1.0594630943592952645618252949463;

FundFreq::FundFreq( ObjectNode * config, int nframe, int nfft, int fs )
: c_nframe( nframe ), c_nfft( nfft ), c_Fs(fs)
{
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT32,c_range,config);
	c_range = VALUENODE_GET_VALUE(c_range);

	SIMPLEVALUENODE_DECLARE_AND_GET(double,c_d,config);
	c_d = VALUENODE_GET_VALUE(c_d);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT32,c_histlen,config);
	c_histlen = VALUENODE_GET_VALUE(c_histlen);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT32,c_considerlen,config);
	c_considerlen = VALUENODE_GET_VALUE(c_considerlen);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT32,c_nstrongest,config);
	c_nstrongest = VALUENODE_GET_VALUE(c_nstrongest);

	m_pHistory = new F0History( c_histlen * (c_nstrongest+1) );

	m_pSortInd = new Q_UINT16[c_nfft];
	m_pFVector = new double[c_considerlen];
	m_pAVector = new double[c_considerlen];

	m_pRho = new double[c_nframe];
}

FundFreq::~FundFreq()
{
	if( m_pHistory )
		delete m_pHistory;
	if( m_pSortInd )
		delete[] m_pSortInd;
	if( m_pFVector )
		delete[] m_pFVector;
	if( m_pAVector )
		delete[] m_pAVector;
	if( m_pRho )
		delete[] m_pRho;
}

void FundFreq::findF0( double * pFrame, double * pF, double * pA, int len, double * f0, double * reability )
{
	double f0_estimation = 0;
	double f0_raw = 0;
	double f0_exact = 0;
	double f0_reability = 0;

	f0_estimation = estimate( pF, pA, len );

	if( f0_estimation > 0 )
		correct( pFrame, f0_estimation, &f0_raw, &f0_exact, &f0_reability );

	*f0 = f0_exact;
	*reability = f0_reability;
}

double FundFreq::estimate( double * pF, double * pA, int len )
{
	int i,j,n; // Indices

	// Get the amplitude/frequency pair for the c_considerlen strongest amplitudes.
	// Because we need only c_considerlen values we do a partial sort.
	IndCompare compobj( pA );
	for( i = 0; i < len; i++ )
	{
		m_pSortInd[i] = i;
	}

	std::partial_sort( m_pSortInd, m_pSortInd + c_considerlen, m_pSortInd + len, compobj );

    for( i = 0; i < c_considerlen; i++ )
	{
		m_pFVector[i] = pF[ m_pSortInd[c_considerlen - i -1] ];
		m_pAVector[i] = pA[ m_pSortInd[c_considerlen - i -1] ];
	}

	// For the c_nstrongest frequencies estimate a base frequency
	for( n = 0; n < c_nstrongest; n++ )
	{
		// Reference frequency
		double f_ref = m_pFVector[c_considerlen-n-1];

		// Skip the estimation if the amplitude of this frequency is really strong
		if( m_pAVector[c_considerlen-1] < 10*m_pAVector[c_considerlen-2] )
		{
			// Calculate a probability value for a denominator and pick the most probable.
			double maxval = -1;
			int den;
			for( j = 0; j < c_range; j++ )
			{
				double den_p_sum = 0;
				for( i = 0; i < c_considerlen; i++ )
				{
					double den_p;
					den_p = m_pFVector[i] / f_ref * (j+1);
					den_p = den_p - floor( den_p + 0.5 );
					den_p = exp( - den_p * den_p / c_d );
					den_p = den_p * m_pAVector[i];
					den_p_sum += den_p;
				}
				if(maxval < den_p_sum)
				{
					maxval = den_p_sum;
					den = j + 1;
				}
			}

			// Now calculate the base frecuency estimation, verify the result
			// and add it to the history
			double f0 = f_ref / den;
			if( f0 >= 80 &&
				f0 < c_fac_one_tone * c_fac_one_tone * m_pFVector[c_considerlen - 1] )
			{
				m_pHistory->insert( f0 );
			}
			else
			{
				m_pHistory->insert_nan();
			}
		}
		else
		{
			// Insert frequency of strongest amplitude
			m_pHistory->insert( m_pFVector[c_considerlen-1] );
		}
	}

	// Get the median from the values in the history for the final esimation
	// Add this estimation back to the history
	double median = m_pHistory->median();
	if( median > 0 )
	{
		m_pHistory->insert( median );
	}
	else
	{
		m_pHistory->insert_nan();
		median = 0;
	}
	return median;
}

void FundFreq::correct( double * pFrame, double f0_e, double * f0_raw, double * f0_exact, double * f0_p )
{
	int nperiod; // number of periods to consider
	int mid; // index of the middle of the frame
	int min_T; // lenght of period of the highest frequency to consider
	int max_T; // length of period of the lowest frequency to consider

	// Define the calculation range (+/- 1 tone from f0_e)
	// and find out how many periods can be considered
	mid = (int)ceil( c_nframe / 2.0 );
	max_T = (int)ceil( c_Fs / (f0_e / c_fac_one_tone) );
	min_T = (int)floor( c_Fs / (f0_e * c_fac_one_tone) );

	if( max_T * 8 <= c_nframe )
		nperiod = 4;
	else if( max_T * 6 <= c_nframe )
		nperiod = 3;
	else if( max_T * 4 <= c_nframe )
		nperiod = 2;
	else if( max_T * 2 <= c_nframe )
		nperiod = 1;
	else
	{
		// Return if we have no chance to correlat over one period
		*f0_raw = 0;
		*f0_exact = 0;
		*f0_p = 0;
		return;
	}

	// For each period calculate the correlation coeffitient
	for( int n = min_T; n <= max_T; n++ )
	{
		int T = n*nperiod;

		double * pLV = &pFrame[mid - T];
		double * pRV = &pFrame[mid];

		// Calculate dot product, and norm
		double dotprod = cblas_ddot(T,pLV,1,pRV,1);
		double leftnorm = cblas_dnrm2(T,pLV,1);
		double rightnorm = cblas_dnrm2(T,pRV,1);

		// Calculate correlation coefficient
		m_pRho[n - min_T] = dotprod / ( leftnorm * rightnorm );
	}
	int rho_len = max_T - min_T + 1;

	// Take the maximum in the correlation result as a raw
	// fundamental frequency and use the coeffitient as a
	// reability measure
	int maxind;
	maxind = cblas_idamax(rho_len,m_pRho,1);

	*f0_raw = (double)c_Fs / (double)( maxind + min_T );
	*f0_p = m_pRho[maxind];

	// Finally refine the raw result with a quadratic interpolation
	double nom, den, temp, polymaxind;
	int offset;

	if( maxind == 0 )
		offset = 0;
	else if( maxind == rho_len - 1 )
		offset = -2;
	else
		offset = -1;

	temp = (min_T+maxind+offset+0)*(m_pRho[maxind+offset+1]-m_pRho[maxind+offset+2]);
	nom = temp * (min_T+maxind+offset+0);
	den = temp;
	temp = (min_T+maxind+offset+1)*(m_pRho[maxind+offset+2]-m_pRho[maxind+offset+0]);
	nom += temp * (min_T+maxind+offset+1);
	den += temp;
	temp = (min_T+maxind+offset+2)*(m_pRho[maxind+offset+0]-m_pRho[maxind+offset+1]);
	nom += temp * (min_T+maxind+offset+2);
	den += temp;

	polymaxind = nom/(2*den);

	// Return the fundamental frequency
	*f0_exact = (double)c_Fs / polymaxind;
}

FundFreq::F0History::F0History( int histlen )
: slength( 0 ), sortedlisthead(NULL), historylisthead(NULL), historylisttail(NULL)
{
	// Fill history list with dummy elements
	listobj * temp = NULL;
	for( int i = 0; i < histlen; i++ )
	{
		if( historylisthead == NULL )
		{
			historylisthead = new listobj( 0, false );
			temp = historylisthead;
		}
		else
		{
			temp->hnext = new listobj( 0, false );
			temp->hnext->hprev = temp;
			temp = temp->hnext;

		}
	}
	historylisttail = temp;
}

FundFreq::F0History::~F0History()
{
	while( historylisthead != NULL )
	{
		listobj * temp = historylisthead;
		historylisthead = historylisthead->hnext;
		delete temp;
	}
}

void FundFreq::F0History::insert( double value )
{
	// Create new valid list object
	listobj * temp = takelastout();
	temp->value = value;
	temp->isvalid = true;
	slength++;

	// Add to the front of history list
	temp->hnext = historylisthead;
	historylisthead->hprev = temp;
	historylisthead = temp;

	// Add to sorted list
	if( sortedlisthead != NULL )
	{
		// Sorted list contains elements, search for insert place
		listobj * it = NULL;
		for( it = sortedlisthead; it->snext != NULL && it->value < value; it = it->snext )
			;

		// Test abort condition
		if( it->value >= value )
		{
			// greater value was found, insert before
			temp->snext = it;
			temp->sprev = it->sprev;
			it->sprev = temp;
			if( temp->sprev != NULL )
				temp->sprev->snext = temp;
			else
				sortedlisthead = temp;
		}
		else
		{
			// reached end  of list, add at the end
			temp->sprev = it;
			temp->snext = NULL;
			it->snext = temp;
		}
	}
	else
		// Sorted list empty, add element
		sortedlisthead = temp;
}

void FundFreq::F0History::insert_nan()
{
	// Create new dummy list object
	listobj * temp = takelastout();

	// Add to the front of history list
	temp->hnext = historylisthead;
	historylisthead->hprev = temp;
	historylisthead = temp;
}

FundFreq::F0History::listobj* FundFreq::F0History::takelastout()
{
	// Is item member of sorted list?
	if( historylisttail->isvalid )
	{
		// Yes, remove from sorted list
		slength--;

		// Adjust forward connection
		if( historylisttail->sprev != NULL )
			historylisttail->sprev->snext = historylisttail->snext;
		else
			sortedlisthead = historylisttail->snext;

		// Adjust backward connection
		if( historylisttail->snext != NULL )
			historylisttail->snext->sprev = historylisttail->sprev;
	}

	// Remove item from history list
	historylisttail->hprev->hnext = NULL;
	listobj * ret = historylisttail;
	historylisttail = historylisttail->hprev;

	// Reset item and return it
	ret->hnext = NULL;
	ret->hprev = NULL;
	ret->snext = NULL;
	ret->sprev = NULL;
	ret->isvalid = false;
	ret->value = 0;
	return ret;
}

double FundFreq::F0History::median()
{
	int mid1 = (int)floor( slength / 2.0 );
	int mid2 = (int)ceil( slength / 2.0 );

	if( slength > 1 )
	{
		listobj * it = sortedlisthead;
		for( int i = 0; i < mid1; i++, it = it->snext )
			;

		if( mid1 == mid2 )
			return (it->value + it->sprev->value ) / 2;
		else
			return it->value;
	}
	else if( slength == 1 )
		return sortedlisthead->value;
	else
		return 0;
}
