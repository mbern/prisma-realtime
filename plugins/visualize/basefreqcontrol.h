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

#ifndef BASE_FREQ_CONROL_H
#define BASE_FREQ_CONROL_H

#include "basefreqcontrolbase.h"

#include <qmap.h>
#include <qstring.h>

class ObjectNode;

class BaseFreqControl : public BaseFreqControlBase
{
	Q_OBJECT
public:
	BaseFreqControl( ObjectNode * config, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~BaseFreqControl();
public slots:
	void setBaseFreq( double );
	void sendState();
protected slots:
	void process();
	void newRefTuning( int );

signals:
	void newRefFreq( double );
	void newBaseFreqDisplay( QStringList );
	void newTuning( int );

private:
	struct BFDecomp
	{
		int octave;
		int note;
		int cent;
		double reffreq;
	};

	enum BFStates {BF_STATIC,BF_ONESHOT,BF_AUTO};

	BFDecomp decomp(double f);

	QMap<int,QString> m_NoteNames;

	BFStates m_State;
	double m_BaseFreq;
	double m_RefFreq;
	double m_RefTuning;

	ObjectNode * conf;
};

#endif // BASE_FREQ_CONROL_H
