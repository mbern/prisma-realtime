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

#include "featurecontrol.h"

#include <math.h>

#include <qcombobox.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <q3frame.h>
#include <q3widgetstack.h>

#include "config/node_macro.h"

FeatureControl::FeatureControl( ObjectNode * config, QWidget* parent, const char* name, Qt::WFlags fl )
: FeatureControlBase(parent,name,fl), m_RefFreq(440), conf(config)
{
	SIMPLEVALUENODE_DECLARE_AND_GET(double,t_length,conf);
	m_t_length = VALUENODE_GET_VALUE(t_length);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,type,conf);
	COB_curve_selection->setCurrentItem(VALUENODE_GET_VALUE(type));
	WS_control->raiseWidget(VALUENODE_GET_VALUE(type));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,hide,conf);
	CB_hide_graph->setChecked(VALUENODE_GET_VALUE(hide));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_am,conf);
	CB_enable_maxamp->setChecked(VALUENODE_GET_VALUE(curve_am));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_ar,conf);
	CB_enable_rmsamp->setChecked(VALUENODE_GET_VALUE(curve_ar));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_partial_1,conf);
	CB_enable_partial1->setChecked(VALUENODE_GET_VALUE(curve_partial_1));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_partial_2,conf);
	CB_enable_partial2->setChecked(VALUENODE_GET_VALUE(curve_partial_2));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_partial_3,conf);
	CB_enable_partial3->setChecked(VALUENODE_GET_VALUE(curve_partial_3));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,f0_axis_type,conf);
	BG_f0_freq_axis->setButton(VALUENODE_GET_VALUE(f0_axis_type));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,f0_fmin,conf);
	SB_f0_start_freq->setValue(VALUENODE_GET_VALUE(f0_fmin));
	SL_f0_start_freq->setValue(VALUENODE_GET_VALUE(f0_fmin));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,f0_fmax,conf);
	SB_f0_stop_freq->setValue(VALUENODE_GET_VALUE(f0_fmax));
	SL_f0_stop_freq->setValue(VALUENODE_GET_VALUE(f0_fmax));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,f0_cent,conf);
	SB_f0_delta_C->setValue(VALUENODE_GET_VALUE(f0_cent));
	SL_f0_delta_C->setValue(VALUENODE_GET_VALUE(f0_cent));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,f0_reffreq,conf);
	CB_f0_show_reffreq->setChecked(VALUENODE_GET_VALUE(f0_reffreq));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,ampl_amin,conf);
	SB_ampl_amp_min->setValue(VALUENODE_GET_VALUE(ampl_amin));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,ampl_amax,conf);
	SB_ampl_amp_max->setValue(VALUENODE_GET_VALUE(ampl_amax));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,partial_amin,conf);
	SB_partial_amp_min->setValue(VALUENODE_GET_VALUE(partial_amin));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,partial_amax,conf);
	SB_partial_amp_max->setValue(VALUENODE_GET_VALUE(partial_amax));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,spc_fmin,conf);
	SB_spc_start_freq->setValue(VALUENODE_GET_VALUE(spc_fmin));
	SL_spc_start_freq->setValue(VALUENODE_GET_VALUE(spc_fmin));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,spc_fmax,conf);
	SB_spc_stop_freq->setValue(VALUENODE_GET_VALUE(f0_fmax));
	SL_spc_stop_freq->setValue(VALUENODE_GET_VALUE(f0_fmax));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,hsl_min,conf);
	SB_hsl_min->setValue(VALUENODE_GET_VALUE(hsl_min));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,hsl_max,conf);
	SB_hsl_max->setValue(VALUENODE_GET_VALUE(hsl_max));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,h2a_min,conf);
	SB_h2a_min->setValue(VALUENODE_GET_VALUE(h2a_min));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,h2a_max,conf);
	SB_h2a_max->setValue(VALUENODE_GET_VALUE(h2a_max));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,int_min,conf);
	SB_int_min->setValue(VALUENODE_GET_VALUE(int_min));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,int_max,conf);
	SB_int_max->setValue(VALUENODE_GET_VALUE(int_max));
}

FeatureControl::~FeatureControl()
{
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,type,conf);
	VALUENODE_SET_VALUE(type,COB_curve_selection->currentItem());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,hide,conf);
	VALUENODE_SET_VALUE(hide,CB_hide_graph->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_am,conf);
	VALUENODE_SET_VALUE(curve_am,CB_enable_maxamp->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_ar,conf);
	VALUENODE_SET_VALUE(curve_ar,CB_enable_rmsamp->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_partial_1,conf);
	VALUENODE_SET_VALUE(curve_partial_1,CB_enable_partial1->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_partial_2,conf);
	VALUENODE_SET_VALUE(curve_partial_2,CB_enable_partial2->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,curve_partial_3,conf);
	VALUENODE_SET_VALUE(curve_partial_3,CB_enable_partial3->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,f0_axis_type,conf);
	VALUENODE_SET_VALUE(f0_axis_type,BG_f0_freq_axis->selectedId());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,f0_fmin,conf);
	VALUENODE_SET_VALUE(f0_fmin,SB_f0_start_freq->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,f0_fmax,conf);
	VALUENODE_SET_VALUE(f0_fmax,SB_f0_stop_freq->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,f0_cent,conf);
	VALUENODE_SET_VALUE(f0_cent,SB_f0_delta_C->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,f0_reffreq,conf);
	VALUENODE_SET_VALUE(f0_reffreq,CB_f0_show_reffreq->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,ampl_amin,conf);
	VALUENODE_SET_VALUE(ampl_amin,SB_ampl_amp_min->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,ampl_amax,conf);
	VALUENODE_SET_VALUE(ampl_amax,SB_ampl_amp_max->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,partial_amin,conf);
	VALUENODE_SET_VALUE(partial_amin,SB_partial_amp_min->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,partial_amax,conf);
	VALUENODE_SET_VALUE(partial_amax,SB_partial_amp_max->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,spc_fmin,conf);
	VALUENODE_SET_VALUE(spc_fmin,SB_spc_start_freq->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,spc_fmax,conf);
	VALUENODE_SET_VALUE(spc_fmax,SB_spc_stop_freq->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,hsl_min,conf);
	VALUENODE_SET_VALUE(hsl_min,SB_hsl_min->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,hsl_max,conf);
	VALUENODE_SET_VALUE(hsl_max,SB_hsl_max->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,h2a_min,conf);
	VALUENODE_SET_VALUE(h2a_min,SB_h2a_min->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,h2a_max,conf);
	VALUENODE_SET_VALUE(h2a_max,SB_h2a_max->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,int_min,conf);
	VALUENODE_SET_VALUE(int_min,SB_int_min->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,int_max,conf);
	VALUENODE_SET_VALUE(int_max,SB_int_max->value());
}

void FeatureControl::setRefFreq( double reffreq )
{
	if(reffreq != m_RefFreq)
	{
		m_RefFreq = reffreq;

		if( COB_curve_selection->currentItem() == SF_BASE_FREQ &&
			BG_f0_freq_axis->selectedId() == SF_F_REF )
		{
			updateGraph();
		}
	}
}

void FeatureControl::updateGraph()
{
	double val;

	ShorttimeFeatureState state;
	GraphState & gstate = state.gstate;

	// Clean the state
	memset(&state, 0, sizeof(ShorttimeFeatureState));

	// Default format of y axis
	gstate.yFracDigit = 0;

	// Configure according the selectet graph type
	switch( COB_curve_selection->currentItem() )
	{
	case SF_BASE_FREQ:
		// Select curve
		state.curves.mask[PV_IDX_ANA_SF_FUND_FREQ] = 1;

		switch( BG_f0_freq_axis->selectedId() )
		{
		case SF_F_MINMAX:
			// Enable self and disable others
			FR_f0_freq_controls->setEnabled(true);
			FR_f0_ref_controls->setDisabled(true);

			// Limit controls
			SB_f0_start_freq->setMaxValue( SB_f0_stop_freq->value() - 1);
			SB_f0_stop_freq->setMinValue( SB_f0_start_freq->value() + 1);

			// Configrue the y-axis
			gstate.yLow = SB_f0_start_freq->value();
			gstate.yHigh = SB_f0_stop_freq->value();
			break;

		case SF_F_REF:
			// Enable self and disable others
			FR_f0_freq_controls->setDisabled(true);
			FR_f0_ref_controls->setEnabled(true);

			// Configrue the y-axis
			val = pow(2.0,(double)SB_f0_delta_C->value() / 2400.0);
			gstate.yLow = floor(m_RefFreq/val);
			gstate.yHigh = ceil(m_RefFreq*val);
			break;

		default:
			qWarning( "Selection does not exist." );
			return;
		}
		break;

	case SF_AMP_DB:
		// Fill in the selected curves
		if( CB_enable_maxamp->isChecked() )
			state.curves.mask[PV_IDX_ANA_SF_MAX_AMP] = 1;
		if( CB_enable_rmsamp->isChecked() )
			state.curves.mask[PV_IDX_ANA_SF_RMS_AMP] = 1;

		// Limit controls
		SB_ampl_amp_min->setMaxValue( SB_ampl_amp_max->value() - 10);
		SB_ampl_amp_max->setMinValue( SB_ampl_amp_min->value() + 10);

		// Configure the y-axis
		gstate.yLow = SB_ampl_amp_min->value();
		gstate.yHigh = SB_ampl_amp_max->value();
		break;

	case SF_LMR:
		// Select curve
		state.curves.mask[PV_IDX_ANA_SF_LOCAL_MAXIMA_RATE] = 1;

		// Configure the y axis
		gstate.yLow = 0;
		gstate.yHigh = 1;
		gstate.yFracDigit = 1;
		break;

	case SF_SPC:
		// Select curve
		state.curves.mask[PV_IDX_ANA_SF_FULL_SPECTRAL_CENTROID] = 1;

		// Limit controls
		SB_spc_start_freq->setMaxValue( SB_spc_stop_freq->value() - 1);
		SB_spc_stop_freq->setMinValue( SB_spc_start_freq->value() + 1);

		// Configure the y-axis
		gstate.yLow = SB_spc_start_freq->value();
		gstate.yHigh = SB_spc_stop_freq->value();
		break;

	case SF_PARTIALS:
		// Fill in the selected curves
		if(CB_enable_partial1->isChecked())
			state.curves.mask[PV_IDX_ANA_SF_PARTIAL_1] = 1;
		if(CB_enable_partial2->isChecked())
			state.curves.mask[PV_IDX_ANA_SF_PARTIAL_2] = 1;
		if(CB_enable_partial3->isChecked())
			state.curves.mask[PV_IDX_ANA_SF_PARTIAL_3] = 1;

		// Limit controls
		SB_partial_amp_min->setMaxValue( SB_partial_amp_max->value() - 10);
		SB_partial_amp_max->setMinValue( SB_partial_amp_min->value() + 10);

		// Configure the y-axis
		gstate.yLow = SB_partial_amp_min->value();
		gstate.yHigh = SB_partial_amp_max->value();
		break;

	case SF_HITOALL:
		// Select curve
		state.curves.mask[PV_IDX_ANA_SF_HI_TO_ALL] = 1;

		// Limit controls
		SB_h2a_min->setMaxValue( SB_h2a_max->value() - 10);
		SB_h2a_max->setMinValue( SB_h2a_min->value() + 10);

		// Configure the y-axis
		gstate.yLow = SB_h2a_min->value();
		gstate.yHigh = SB_h2a_max->value();
		break;

	case SF_HSL:
		// Select curve
		state.curves.mask[PV_IDX_ANA_SF_HARMONIC_SLOPE] = 1;

		// Limit controls
		SB_hsl_min->setMaxValue( SB_hsl_max->value() - 1);
		SB_hsl_max->setMinValue( SB_hsl_min->value() + 1);

		// Configure the y-axis
		gstate.yLow = SB_hsl_min->value();
		gstate.yHigh = SB_hsl_max->value();
		if(gstate.yHigh - gstate.yLow < 5)
			gstate.yFracDigit = 1;
		break;

	case SF_INTONATION:
		// Select curve
		state.curves.mask[PV_IDX_ANA_SF_INTONATION] = 1;

		// Configure the y-axis
		gstate.yLow = SB_int_min->value();
		gstate.yHigh = SB_int_max->value();
		break;

	default:
		qWarning( "Selection does not exist." );
		return;
	}

	// Configure x-axis
	gstate.xLow = - m_t_length;
	gstate.xHigh = 0;
	gstate.xFracDigit = 1;

	// Set visibility
	gstate.isVisible = !CB_hide_graph->isChecked();

	// Call marker update
	showRefTone();

	emit newState(state);
}

void FeatureControl::showRefTone()
{
	if( COB_curve_selection->currentItem() == SF_BASE_FREQ && CB_f0_show_reffreq->isChecked() )
		emit showToneMarker( true );
	else
		emit showToneMarker( false );
}
