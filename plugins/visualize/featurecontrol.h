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

#ifndef FEATURE_CONTROL_H
#define FEATURE_CONTROL_H

#include "featurecontrolbase.h"

#include "graphdefs.h"

class ObjectNode;

enum ShorttimeFeatureViews {
	SF_BASE_FREQ,
	SF_INTONATION,
	SF_AMP_DB,
	SF_PARTIALS,
	SF_SPC,
	SF_HITOALL,
	SF_HSL,
	SF_LMR
};
enum SFFreqRange { SF_F_MINMAX, SF_F_REF };

class FeatureControl : public FeatureControlBase
{
	Q_OBJECT
public:
	FeatureControl( ObjectNode * config, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~FeatureControl();

public slots:
	void sendState()
	{
		updateGraph();
	}

	void setRefFreq( double reffreq );

signals:
	void newState( const ShorttimeFeatureState & state );
	void showToneMarker( bool show );

protected slots:
	virtual void updateGraph();
	virtual void showRefTone();

private:
	double m_t_length;
	double m_RefFreq;
	ObjectNode * conf;
};

#endif //FEATURE_CONTROL_H
