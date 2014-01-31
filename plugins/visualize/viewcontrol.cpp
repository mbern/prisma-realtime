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

#include "viewcontrol.h"

#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <q3frame.h>
#include <q3buttongroup.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qslider.h>

#include "config/node_macro.h"

#include <math.h>

ViewControl::ViewControl( ObjectNode * config, int views, bool hasbypass, QWidget* parent, const char* name, Qt::WFlags fl )
: ViewControlBase(parent,name,fl), m_RefFreq(440), m_Bypass(hasbypass), m_LastGroupID(0), conf(config)
{
	if( views & FA_Atlas )
	{
		ComboBoxViewSelection->insertItem( QString( "Frequenz-Amplitude" ) );
		m_ComboBoxView.append( FA_Atlas );
	}
	if( views & TF_Atlas )
	{
		ComboBoxViewSelection->insertItem( QString( "Zeit-Frequenz" ) );
		m_ComboBoxView.append( TF_Atlas );
	}
	if( views & TA_Atlas )
	{
		ComboBoxViewSelection->insertItem( QString( "Zeit-Amplitude" ) );
		m_ComboBoxView.append( TA_Atlas );
	}
	if( views & TF_Spectrum )
	{
		ComboBoxViewSelection->insertItem( QString( "Spektrogramm" ) );
		m_ComboBoxView.append( TF_Spectrum );
	}

	m_ViewState.isVisible = true;

	SIMPLEVALUENODE_DECLARE_AND_GET(double,t_length,conf);
	m_ViewState.tLow = - VALUENODE_GET_VALUE(t_length);
	m_ViewState.tHigh = 0;

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,type,conf);
	ComboBoxViewSelection->setCurrentItem(VALUENODE_GET_VALUE(type));

	if(m_Bypass)
	{
		connect( CheckBoxBypass, SIGNAL( toggled(bool) ), SLOT( getState() ) );

		SIMPLEVALUENODE_DECLARE_AND_GET(bool,sync,conf);
		CheckBoxBypass->setChecked(VALUENODE_GET_VALUE(sync));
	}
	else
	{
		CheckBoxBypass->hide();
	}

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,hide,conf);
	CheckBoxHide->setChecked(VALUENODE_GET_VALUE(hide));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,fselection,conf);
	ButtonGroupFrequenz->setButton(VALUENODE_GET_VALUE(fselection));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,fmin,conf);
	SpinBoxStartFreq->setValue(VALUENODE_GET_VALUE(fmin));
	SliderStartFreq->setValue(VALUENODE_GET_VALUE(fmin));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,fmax,conf);
	SpinBoxStopFreq->setValue(VALUENODE_GET_VALUE(fmax));
	SliderStopFreq->setValue(VALUENODE_GET_VALUE(fmax));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,cent,conf);
	SpinBoxDeltaC->setValue(VALUENODE_GET_VALUE(cent));
	SliderDeltaC->setValue(VALUENODE_GET_VALUE(cent));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,harmonic,conf);
	SpinBoxPartial->setValue(VALUENODE_GET_VALUE(harmonic));
	SliderPartial->setValue(VALUENODE_GET_VALUE(harmonic));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,showfrom1,conf);
	CheckBoxAllPartial->setChecked(VALUENODE_GET_VALUE(showfrom1));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,amin,conf);
	SpinBoxMinA->setValue(VALUENODE_GET_VALUE(amin));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,amax,conf);
	SpinBoxMaxA->setValue(VALUENODE_GET_VALUE(amax));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,reffreq,conf);
	CheckBoxBezugston->setChecked(VALUENODE_GET_VALUE(reffreq));

	emit calculate();
}

ViewControl::~ViewControl()
{
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,type,conf);
	VALUENODE_SET_VALUE(type,ComboBoxViewSelection->currentItem());

	if(m_Bypass)
	{
		SIMPLEVALUENODE_DECLARE_AND_GET(bool,sync,conf);
		VALUENODE_SET_VALUE(sync,CheckBoxBypass->isChecked());
	}

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,hide,conf);
	VALUENODE_SET_VALUE(hide,CheckBoxHide->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,fselection,conf);
	VALUENODE_SET_VALUE(fselection,ButtonGroupFrequenz->selectedId());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,fmin,conf);
	VALUENODE_SET_VALUE(fmin,SpinBoxStartFreq->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,fmax,conf);
	VALUENODE_SET_VALUE(fmax,SpinBoxStopFreq->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,cent,conf);
	VALUENODE_SET_VALUE(cent,SpinBoxDeltaC->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,harmonic,conf);
	VALUENODE_SET_VALUE(harmonic,SpinBoxPartial->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,showfrom1,conf);
	VALUENODE_SET_VALUE(showfrom1,CheckBoxAllPartial->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,amin,conf);
	VALUENODE_SET_VALUE(amin,SpinBoxMinA->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,amax,conf);
	VALUENODE_SET_VALUE(amax,SpinBoxMaxA->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,reffreq,conf);
	VALUENODE_SET_VALUE(reffreq,CheckBoxBezugston->isChecked());
}

void ViewControl::setRefFreq( double reffreq )
{
	if( !CheckBoxBypass->isChecked() )
	{
		m_RefFreq = reffreq;
		if( ButtonGroupFrequenz->selectedId() == 1 )
		{
			emit calculateF();
		}
	}
}

void ViewControl::enableViewSelection( bool b )
{
	if(b)
	{
		GroupBoxDiagram->show();
	}
	else
	{
		GroupBoxDiagram->hide();
	}
}

void ViewControl::newBypassData( const ViewState & state )
{
	if(CheckBoxBypass->isChecked())
	{
		m_ViewState = state;
		changeType();
	}
}

void ViewControl::sendState()
{
	emit newFLow( m_ViewState.fLow );
	emit newFHigh( m_ViewState.fHigh );
	emit newALow( m_ViewState.aLow );
	emit newAHigh( m_ViewState.aHigh );
	changeType();
}


void ViewControl::calculate()
{
	blockSignals( true );

	calculateF();
	calculateA();
	changeType();

	blockSignals( false );

	emit newState( m_ViewState );
}

void ViewControl::calculateF()
{
	double flow = 0;
	double fhigh = 20000;
	int groupID = ButtonGroupFrequenz->selectedId();
	switch( groupID )
	{
	case 0:
		flow = SpinBoxStartFreq->value();
		fhigh = SpinBoxStopFreq->value();
		SpinBoxStopFreq->setMinValue((int)(flow + 1));
		SpinBoxStartFreq->setMaxValue((int)(fhigh - 1));
		if( m_LastGroupID == 1 ) { emit partialsEnabled( false ); };
		break;
	case 1:
		{
			double bwfactor = pow(1.0005777895065548592967925757932,SpinBoxDeltaC->value());
			fhigh = (SpinBoxPartial->value())*m_RefFreq*bwfactor;
			if(CheckBoxAllPartial->isChecked())
				flow = 0;
			else
				flow = (SpinBoxPartial->value())*m_RefFreq/bwfactor;
			if( m_LastGroupID != 1 ) { emit partialsEnabled( true ); };
		}
		break;
	}
	m_LastGroupID = groupID;

	if( m_ViewState.fLow != flow )
		emit newFLow( flow );

	if( m_ViewState.fHigh != fhigh )
		emit newFHigh( fhigh );

	m_ViewState.fLow = flow;
	m_ViewState.fHigh = fhigh;

	emit newState( m_ViewState );
}

void ViewControl::calculateA()
{
	SpinBoxMaxA->setMinValue(SpinBoxMinA->value() + 10);
	SpinBoxMinA->setMaxValue(SpinBoxMaxA->value() - 10);

	double ahigh = SpinBoxMaxA->value();
	double alow = SpinBoxMinA->value();

	if( m_ViewState.aHigh != ahigh )
		emit newAHigh( ahigh );

	if( m_ViewState.aLow != alow )
		emit newALow( alow );

	m_ViewState.aHigh = ahigh;
	m_ViewState.aLow = alow;

	emit newState( m_ViewState );
}

void ViewControl::changeType()
{
	m_ViewState.eView = m_ComboBoxView[ComboBoxViewSelection->currentItem()];
	m_ViewState.isVisible = !CheckBoxHide->isChecked();

	switch(m_ViewState.eView)
	{
	case FA_Atlas:
		CheckBoxBezugston->setDisabled(false);
		emit setToneMarkerDir( Marker::M_V );
		break;
	case TF_Atlas:
	case TF_Spectrum:
		CheckBoxBezugston->setDisabled(false);
		emit setToneMarkerDir( Marker::M_H );
		break;
	case TA_Atlas:
		CheckBoxBezugston->setDisabled(true);
		break;
	}
	showToneMarkerClicked();

	emit newState( m_ViewState );
}

void ViewControl::showToneMarkerClicked()
{
	if( CheckBoxBezugston->isEnabled() )
	{
		if( CheckBoxBezugston->isChecked() )
			emit showToneMarker(true);
		else
			emit showToneMarker(false);
	}
	else
	{
		emit showToneMarker(false);
	}
}
