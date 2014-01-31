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

#ifndef VIEW_CONTROL_H
#define VIEW_CONTROL_H

#include <qwidget.h>
#include <q3valuevector.h>

#include "spectrumgraph.h"
#include "viewcontrolbase.h"

class ObjectNode;

class ViewControl : public ViewControlBase
{
	Q_OBJECT
public:
	ViewControl( ObjectNode * config, int views = FA_Atlas, bool hasbypass = FALSE, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~ViewControl();
public slots:
	void setRefFreq( double reffreq );
	void enableViewSelection( bool b );
	void newBypassData( const ViewState & state );
	void calculate();
	void sendState();
signals:
	void partialsEnabled( bool b );
	void newState( const ViewState & state );
	void trigger( );
	void newFLow( double f );
	void newFHigh( double f );
	void newALow( double a );
	void newAHigh( double a );
	void showToneMarker( bool b );
	void setToneMarkerDir( int d );
protected slots:
	void calculateF();
	void calculateA();
	void changeType();
	void getState() { emit trigger(); }
	void hideGraph( bool h ) { m_ViewState.isVisible = !h; emit newState( m_ViewState ); }
	void showToneMarkerClicked();
private:
	double m_RefFreq;

	bool m_Bypass;
	bool m_ViewSelection;

	int m_LastGroupID;

	Q3ValueVector<View> m_ComboBoxView;

	ViewState m_ViewState;

	ObjectNode * conf;
};

#endif //VIEW_CONTROL_H
