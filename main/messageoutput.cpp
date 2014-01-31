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

#include "messageoutput.h"

// For abort()
#include <stdlib.h>

#include <qfile.h>
#include <q3textstream.h>
#include <qdatetime.h>
#include <qstring.h>

MyMessageOutput * MyMessageOutput::myself = NULL;

MyMessageOutput::MyMessageOutput( const QString & filename, unsigned int max_size )
: m_logfile(NULL), m_stream(NULL)
{
	QIODevice::OpenMode open_flags = QIODevice::WriteOnly;

	// Set static pointer
	if(myself)
		delete myself;
	myself = this;

	// Create file object
	m_logfile = new QFile(filename);

	// Set the open flags. Check if the file is not too large to append
	if(m_logfile->size() < max_size)
		open_flags |= QIODevice::Append;
	else
		open_flags |= QIODevice::Truncate;

	// Try to open file
	if(!m_logfile->open(open_flags))
	{
		qInstallMsgHandler(0);
		qWarning("Could not open file '%s'. Use default handler",filename.ascii());
		return;
	}

	// Create text stream
	m_stream = new Q3TextStream( m_logfile );

	// Emit time stamp
	*m_stream << "--- " << QDateTime::currentDateTime().toString(Qt::LocalDate) << " ---\n";

	// Install Handler
	qInstallMsgHandler(MyMessageOutput::handler);
}

MyMessageOutput::~MyMessageOutput( )
{
	// Revert handler to default
	qInstallMsgHandler(0);

	delete m_stream;

	m_logfile->close();

	delete m_logfile;

	myself = NULL;
}

inline void MyMessageOutput::debugMsg( const char *msg )
{
	*m_stream << "Debug   : " << msg << "\n";
}

inline void MyMessageOutput::warningMsg( const char *msg )
{
	*m_stream << "Warning : " << msg << "\n";
}

inline void MyMessageOutput::fatalMsg( const char *msg )
{
	*m_stream << "Fatal   : " << msg << "\n";
	m_logfile->flush();
}

void MyMessageOutput::handler( QtMsgType type, const char *msg )
{
	if(myself != NULL && myself->m_stream == NULL)
	{
		qInstallMsgHandler(0);
		qWarning("Custom message handler is not initialized. Use default handler");
		switch(type)
		{
		case QtDebugMsg:
			qDebug("%s", msg);
			break;
		case QtWarningMsg:
			qWarning("%s", msg);
			break;
		default:
			qFatal("%s", msg);
			break;
		}
	}

	switch ( type )
	{
	case QtDebugMsg:
		myself->debugMsg( msg );
		break;
	case QtWarningMsg:
		myself->warningMsg( msg );
		break;
	default:
		myself->fatalMsg( msg );
		abort();                    // deliberately core dump
		break;
	}
}
