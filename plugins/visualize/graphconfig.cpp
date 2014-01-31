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

#include "graphconfig.h"

#include <qspinbox.h>
#include <qlabel.h>

#include "config/node_macro.h"

GraphConfig::GraphConfig( ObjectNode * config, QWidget* parent, const char* name, Qt::WFlags fl )
: GraphConfigBase(parent,name,fl),conf(config)
{
	QString num;

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,deltaC,conf);
	SpinBoxBandwidth->setValue(VALUENODE_GET_VALUE(deltaC));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,pointSize,conf);
	SpinBoxPointsize->setValue(VALUENODE_GET_VALUE(pointSize));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,backgroundGray,conf);
	SpinBoxGray->setValue(VALUENODE_GET_VALUE(backgroundGray));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,fs,conf);
	num.setNum(VALUENODE_GET_VALUE(fs));
	TL_basefreq->setText(num);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,frame_size,conf);
	num.setNum(VALUENODE_GET_VALUE(frame_size));
	TL_windowsize->setText(num);

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,hop_size,conf);
	num.setNum(VALUENODE_GET_VALUE(hop_size));
	TL_hopsize->setText(num);
}

GraphConfig::~GraphConfig()
{
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,deltaC,conf);
	VALUENODE_SET_VALUE(deltaC,SpinBoxBandwidth->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,pointSize,conf);
	VALUENODE_SET_VALUE(pointSize,SpinBoxPointsize->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT8,backgroundGray,conf);
	VALUENODE_SET_VALUE(backgroundGray,SpinBoxGray->value());
}

void GraphConfig::deltaC( int v )
{
	emit deltaCChanged(v/2);
}

void GraphConfig::pointSize( int s )
{
	emit pointSizeChanged(s);
}

void GraphConfig::backgroundGray( int g )
{
	emit backgroundGrayChanged((unsigned char)g);
}

void GraphConfig::sendState()
{
	emit deltaCChanged(SpinBoxBandwidth->value()/2);
	emit pointSizeChanged(SpinBoxPointsize->value());
	emit backgroundGrayChanged((unsigned char)SpinBoxGray->value());
}
