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

#ifndef GRAPH_DEFS_H
#define GRAPH_DEFS_H

#include "prisma_defines.h"

enum View { FA_Atlas = 1, TA_Atlas = 2, TF_Atlas = 4, TF_Spectrum = 8 };

typedef struct
{
    char mask[NUM_SHORTTIME_FEATURES];
} t_curves_mask;

struct ViewState
{
	enum View eView;
	double fLow, fHigh;
	double aLow, aHigh;
	double tLow, tHigh;
	bool isVisible;
};

struct GraphState
{
	double xLow, xHigh;
	double yLow, yHigh;
	double z;
	unsigned char xFracDigit;
	unsigned char yFracDigit;
	bool isVisible;
};

struct ShorttimeFeatureState
{
	t_curves_mask curves;
	struct GraphState gstate;
};

#endif // GRAPH_DEFS_H
