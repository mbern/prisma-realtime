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

#ifndef GRAPH_CONFIG_BASE_H
#define GRAPH_CONFIG_BASE_H

#include "graphconfigbase.h"

class ObjectNode;

class GraphConfig : public GraphConfigBase
{
	Q_OBJECT
public:
	GraphConfig( ObjectNode * config, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~GraphConfig();

public slots:
	void sendState();

protected slots:
	void deltaC( int v );
	void pointSize( int s );
	void backgroundGray( int g );

signals:
	void deltaCChanged( int );
	void pointSizeChanged( int );
	void backgroundGrayChanged( unsigned char );

private:
	ObjectNode * conf;
};

#endif // GRAPH_CONFIG_BASE_H
