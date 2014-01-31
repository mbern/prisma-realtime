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

#ifndef FILE_WRITE_CONTROL_H
#define FILE_WRITE_CONTROL_H

#include "qstring.h"
//Added by qt3to4:
#include <QTimerEvent>

#include "filewritecontrolbase.h"

class ObjectNode;

class FileWriteControl : public FileWriteControlBase
{
	Q_OBJECT
public:
	FileWriteControl( ObjectNode * config, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~FileWriteControl();
protected:
	void timerEvent( QTimerEvent *e );
public slots:
	void setPause( bool b );
	void setBufferFilling( int f );
	void fireConfiguration();
private slots:
	void changeBufferLength( int len );
	void selectFolder();
	void saveData();
signals:
	void sendUpdate();
	void sendLength( int );
	void sendDataSave( QString );
	void sendWaveSave( QString );
	void sendCopy();
private:
	QString m_BasePath;
	bool m_isRunning;
	ObjectNode * conf;
};

#endif // FILE_WRITE_CONTROL_H
