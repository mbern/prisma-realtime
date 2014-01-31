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

#ifndef TOP_DISPLAY_H
#define TOP_DISPLAY_H

#include "topdisplaybase.h"

#include <qstringlist.h>
#include <qcolor.h>
#include <qtimer.h>

class ObjectNode;

class TopDisplay : public TopDisplayBase
{
	Q_OBJECT
public:
	TopDisplay( ObjectNode * config, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~TopDisplay();
public slots:
	void setBaseFreqDisplay( QStringList strlist );
	void setMeterValue( double max, double rms );
	void sendState();
protected slots:
	void paused( bool  p );
	void pauseTimerTimeout();
signals:
	void pause(bool);
private:
	QTimer m_pausetimer;
	QColor m_col1,m_col2;
	bool m_cflag;
	ObjectNode * conf;
};

#endif // TOP_DISPLAY_H
