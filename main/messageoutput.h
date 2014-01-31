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

#ifndef MESSAGE_OUTPUT_H
#define MESSAGE_OUTPUT_H

#include <qapplication.h>
//Added by qt3to4:
#include <Q3TextStream>

class QFile;
class Q3TextStream;
class QString;

class MyMessageOutput
{
public:
	MyMessageOutput( const QString & filename, unsigned int max_size = 1048576);
	~MyMessageOutput();

private:
	static void handler( QtMsgType type, const char *msg );
	void debugMsg( const char *msg );
	void warningMsg( const char *msg );
	void fatalMsg( const char *msg );

	static MyMessageOutput* myself;
	QFile *m_logfile;
	Q3TextStream *m_stream;
};

#endif // MESSAGE_OUTPUT_H
