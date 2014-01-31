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

#ifndef PRISMA_DEFINES_H
#define PRISMA_DEFINES_H

#define PT_STR_SHORT "short"
#define PT_STR_DOUBLE "double"

#define PS_STR_AUDIO "audio"
#define PS_IDX_AUDIO 0x00010000

#define PC_STR_AUDIO_16BIT_MONO "audio_16bit_mono"
#define PC_IDX_AUDIO_16BIT_MONO 0

#define PS_STR_ANALYSIS "analysis"
#define PS_IDX_ANALYSIS 0x00020000

#define PC_STR_ANALYSIS_A "analysis_A"
#define PC_IDX_ANALYSIS_A 0
#define PC_STR_ANALYSIS_F "analysis_F"
#define PC_IDX_ANALYSIS_F 1
#define PC_STR_ANALYSIS_SF "analysis_shorttime_feature"
#define PC_IDX_ANALYSIS_SF 2

#define PV_IDX_ANA_SF_FUND_FREQ					 0
#define PV_IDX_ANA_SF_FUND_FREQ_QUAL			 1
#define PV_IDX_ANA_SF_INTONATION				 2
#define PV_IDX_ANA_SF_RMS_AMP					 3
#define PV_IDX_ANA_SF_MAX_AMP					 4
#define PV_IDX_ANA_SF_PARTIAL_1					 5
#define PV_IDX_ANA_SF_PARTIAL_2					 6
#define PV_IDX_ANA_SF_PARTIAL_3					 7
#define PV_IDX_ANA_SF_FULL_SPECTRAL_CENTROID	 8
#define PV_IDX_ANA_SF_HI_TO_ALL					 9
#define PV_IDX_ANA_SF_HARMONIC_SLOPE			10
#define PV_IDX_ANA_SF_LOCAL_MAXIMA_RATE			11

#define LAST_SHORTTIME_FEATURE PV_IDX_ANA_SF_LOCAL_MAXIMA_RATE
#define NUM_SHORTTIME_FEATURES (LAST_SHORTTIME_FEATURE + 1)

#endif // PRISMA_DEFINES_H
