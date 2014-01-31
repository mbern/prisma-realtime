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

#include "topdisplay.h"

#include <qlabel.h>
#include <qpalette.h>
#include <qpushbutton.h>

#include <math.h>

#include "config/node_macro.h"

TopDisplay::TopDisplay( ObjectNode * config, QWidget* parent, const char* name, Qt::WFlags fl )
:TopDisplayBase( parent, name, fl ),conf(config)
{
	const QColorGroup & cg = pushButtonPause->colorGroup();
	m_col1 = cg.background();
	m_col2 = cg.buttonText();
	m_cflag = true;

	this->connect( &m_pausetimer, SIGNAL( timeout() ), SLOT( pauseTimerTimeout() ) );

//	SIMPLEVALUENODE_DECLARE_AND_GET(bool,pause,conf);
//	pushButtonPause->setOn(VALUENODE_GET_VALUE(pause));
}

TopDisplay::~TopDisplay()
{
//	SIMPLEVALUENODE_DECLARE_AND_GET(bool,pause,conf);
//	VALUENODE_SET_VALUE(pause,pushButtonPause->isOn());
}

void TopDisplay::setBaseFreqDisplay( QStringList strlist )
{
	TextLabelCurNote->setText( strlist[0] );
	TextLabelCurSign->setText( strlist[1] );
	TextLabelCurCent->setText( strlist[2] );
	TextLabelCurHz->setText( strlist[3] );
	TextLabelRefFreq->setText( strlist[4] );
}

void TopDisplay::setMeterValue( double max, double rms )
{
	TextLabelMaxValue->setText( QString::number(20*log10(max),'f',1) );
	TextLabelRmsValue->setText( QString::number(20*log10(rms),'f',1) );
}

void TopDisplay::sendState()
{
	paused(pushButtonPause->isOn());
}

void TopDisplay::paused( bool  p )
{
	if(p)
	{
		m_pausetimer.start(400);
		emit pause(true);
	}
	else
	{
		m_pausetimer.stop();
		pushButtonPause->setPaletteForegroundColor( m_col2 );
		emit pause(false);
	}
}


void TopDisplay::pauseTimerTimeout()
{
	if(m_cflag)
	{
		pushButtonPause->setPaletteForegroundColor( m_col1 );
	}
	else
	{
		pushButtonPause->setPaletteForegroundColor( m_col2 );
	}
	m_cflag = !m_cflag;
}
