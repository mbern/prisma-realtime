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

#include "basefreqcontrol.h"

#include <q3buttongroup.h>
#include <qpushbutton.h>
#include <q3frame.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcombobox.h>

#include <math.h>

#include "config/node_macro.h"

BaseFreqControl::BaseFreqControl( ObjectNode * config, QWidget* parent, const char* name, Qt::WFlags fl )
:BaseFreqControlBase( parent, name, fl ), conf(config)
{
	m_NoteNames.insert( -33, "C" );
	m_NoteNames.insert( -32, "Cis" );
	m_NoteNames.insert( -31, "D" );
	m_NoteNames.insert( -30, "Dis" );
	m_NoteNames.insert( -29, "E" );
	m_NoteNames.insert( -28, "F" );
	m_NoteNames.insert( -27, "Fis" );
	m_NoteNames.insert( -26, "G" );
	m_NoteNames.insert( -25, "Gis" );
	m_NoteNames.insert( -24, "A" );
	m_NoteNames.insert( -23, "B" );
	m_NoteNames.insert( -22, "H" );
	m_NoteNames.insert( -21, "c" );
	m_NoteNames.insert( -20, "cis" );
	m_NoteNames.insert( -19, "d" );
	m_NoteNames.insert( -18, "dis" );
	m_NoteNames.insert( -17, "e" );
	m_NoteNames.insert( -16, "f" );
	m_NoteNames.insert( -15, "fis" );
	m_NoteNames.insert( -14, "g" );
	m_NoteNames.insert( -13, "gis" );
	m_NoteNames.insert( -12, "a" );
	m_NoteNames.insert( -11, "b" );
	m_NoteNames.insert( -10, "h" );
	m_NoteNames.insert( -9, "c'" );
	m_NoteNames.insert( -8, "cis'" );
	m_NoteNames.insert( -7, "d'" );
	m_NoteNames.insert( -6, "dis'" );
	m_NoteNames.insert( -5, "e'" );
	m_NoteNames.insert( -4, "f'" );
	m_NoteNames.insert( -3, "fis'" );
	m_NoteNames.insert( -2, "g'" );
	m_NoteNames.insert( -1, "gis'" );
	m_NoteNames.insert( 0, "a'" );
	m_NoteNames.insert( 1, "b'" );
	m_NoteNames.insert( 2, "h'" );
	m_NoteNames.insert( 3, "c''" );
	m_NoteNames.insert( 4, "cis''" );
	m_NoteNames.insert( 5, "d''" );
	m_NoteNames.insert( 6, "dis''" );
	m_NoteNames.insert( 7, "e''" );
	m_NoteNames.insert( 8, "f''" );
	m_NoteNames.insert( 9, "fis''" );
	m_NoteNames.insert( 10, "g''" );
	m_NoteNames.insert( 11, "gis''" );
	m_NoteNames.insert( 12, "a''" );
	m_NoteNames.insert( 13, "b''" );
	m_NoteNames.insert( 14, "h''" );
	m_NoteNames.insert( 15, "c'''" );
	m_NoteNames.insert( 16, "cis'''" );
	m_NoteNames.insert( 17, "d'''" );
	m_NoteNames.insert( 18, "dis'''" );
	m_NoteNames.insert( 19, "e'''" );
	m_NoteNames.insert( 20, "f'''" );
	m_NoteNames.insert( 21, "fis'''" );
	m_NoteNames.insert( 22, "g'''" );
	m_NoteNames.insert( 23, "gis'''" );
	m_NoteNames.insert( 24, "a'''" );
	m_NoteNames.insert( 25, "b'''" );
	m_NoteNames.insert( 26, "h'''" );
	m_NoteNames.insert( 27, "c''''" );
	m_NoteNames.insert( 28, "cis''''" );
	m_NoteNames.insert( 29, "d''''" );
	m_NoteNames.insert( 30, "dis''''" );
	m_NoteNames.insert( 31, "e''''" );
	m_NoteNames.insert( 32, "f''''" );
	m_NoteNames.insert( 33, "fis''''" );
	m_NoteNames.insert( 34, "g''''" );
	m_NoteNames.insert( 35, "gis''''" );
	m_NoteNames.insert( 36, "a''''" );
	m_NoteNames.insert( 37, "b''''" );
	m_NoteNames.insert( 38, "h''''" );

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,note,conf);
	ComboBoxNote->setCurrentItem(VALUENODE_GET_VALUE(note));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT32,octave,conf);
	SpinBoxOctave->setValue(VALUENODE_GET_VALUE(octave));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,tuning,conf);
	SpinBoxTuning->setValue(VALUENODE_GET_VALUE(tuning));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,mode,conf);
	ButtonGroup->setButton(VALUENODE_GET_VALUE(mode));

	SIMPLEVALUENODE_DECLARE_AND_GET(double,refFreq,conf);
	m_RefFreq = VALUENODE_GET_VALUE(refFreq);
	m_BaseFreq = m_RefFreq;

	sendState();
}

BaseFreqControl::~BaseFreqControl()
{
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,note,conf);
	VALUENODE_SET_VALUE(note,ComboBoxNote->currentItem());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT32,octave,conf);
	VALUENODE_SET_VALUE(octave,SpinBoxOctave->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT16,tuning,conf);
	VALUENODE_SET_VALUE(tuning,SpinBoxTuning->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,mode,conf);
	VALUENODE_SET_VALUE(mode,ButtonGroup->selectedId());

	SIMPLEVALUENODE_DECLARE_AND_GET(double,refFreq,conf);
	VALUENODE_SET_VALUE(refFreq,m_RefFreq);
}

void BaseFreqControl::sendState()
{
	m_RefTuning = SpinBoxTuning->value();
	process();
	setBaseFreq( m_BaseFreq );
	emit newTuning( m_RefTuning );
}

void BaseFreqControl::process()
{
	BFDecomp d;

	switch( ButtonGroup->selectedId() )
	{
	case 0:
		m_State = BF_STATIC;
		break;
	case 1:
		m_State = BF_ONESHOT;
		break;
	case 2:
		m_State = BF_AUTO;
		break;
	}

	if( m_State == BF_STATIC )
	{
		d.octave = SpinBoxOctave->value()-1; // Korrektur des Wertebereichs im GUI
		d.note = ComboBoxNote->currentItem();
		d.cent = 0;
		d.reffreq = pow(2.0,(12*d.octave+d.note-9)/12.0)*m_RefTuning;
	}
	else if(m_State == BF_ONESHOT || m_State == BF_AUTO )
	{
		d = decomp(m_BaseFreq);
	}

	TextLabelRefNote->setText( m_NoteNames[12*d.octave+d.note-9] );
	m_RefFreq = d.reffreq;
	emit newRefFreq( m_RefFreq );
}

void BaseFreqControl::newRefTuning( int reftuning )
{
	if( m_State == BF_STATIC || m_State == BF_ONESHOT )
	{
		m_RefFreq = m_RefFreq * (double)reftuning / m_RefTuning;
		m_RefTuning = reftuning;
		emit newRefFreq( m_RefFreq );
	}

	m_RefTuning = reftuning;
	setBaseFreq(m_BaseFreq);
	emit newTuning( reftuning );
}

void BaseFreqControl::setBaseFreq( double basefreq )
{
	BFDecomp d;
	QStringList strlist;

	m_BaseFreq = basefreq;

	d = decomp(m_BaseFreq);

	strlist.append( m_NoteNames[12*d.octave+d.note-9] );
	if( d.cent >= 0 )
	{
		strlist.append( "+" );
		strlist.append( QString::number( d.cent ) );
	}
	else
	{
		strlist.append( "-" );
		strlist.append( QString::number( -d.cent ) );
	}
	strlist.append( QString::number( m_BaseFreq, 'f', 1 ) );
	strlist.append( QString::number( m_RefTuning ) );
	emit newBaseFreqDisplay( strlist );

	if( m_State == BF_AUTO)
	{
		TextLabelRefNote->setText( m_NoteNames[12*d.octave+d.note-9] );
		m_RefFreq = d.reffreq;
		emit newRefFreq( m_RefFreq );
	}
}

BaseFreqControl::BFDecomp BaseFreqControl::decomp(double f)
{
	BFDecomp d;
	int logfreq = (int)floor(log(f/m_RefTuning)*17.312340490667560888319096172023+0.5)+9; // 17.31... = 1/log(2^(1/12))
	d.octave = (int)floor(logfreq / 12.0);
	d.note = logfreq - 12 * d.octave;
	d.reffreq = pow(2.0,(12*d.octave+d.note-9)/12.0)*m_RefTuning;
	d.cent = (int)(log(f/d.reffreq)*1731.2340490667560888319096172023);
	return d;
}
