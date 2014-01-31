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

#include "filewritecontrol.h"

#include "q3groupbox.h"
#include "q3filedialog.h"
#include "qlineedit.h"
#include "q3progressbar.h"
#include "qlabel.h"
#include "qdir.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qcheckbox.h"
#include <qspinbox.h>
//Added by qt3to4:
#include <QTimerEvent>

#include "config/node_macro.h"

FileWriteControl::FileWriteControl( ObjectNode * config, QWidget* parent, const char* name, Qt::WFlags fl )
: FileWriteControlBase(parent,name,fl),conf(config)
{
	m_isRunning = true;

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,buffer_max_length,conf);
	SpinBoxBufferLength->setMaxValue(VALUENODE_GET_VALUE(buffer_max_length));

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,buffer_length,conf);
	SpinBoxBufferLength->setValue(VALUENODE_GET_VALUE(buffer_length));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,save_bin,conf);
	CheckBoxBin->setChecked(VALUENODE_GET_VALUE(save_bin));

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,save_wave,conf);
	CheckBoxWav->setChecked(VALUENODE_GET_VALUE(save_wave));

	OBJECTVALUENODE_DECLARE_AND_GET(QString,save_path,conf);
	if(!VALUENODE_GET_VALUE(save_path).isEmpty())
	{
		m_BasePath = VALUENODE_GET_VALUE(save_path);
	}
	else
	{
		m_BasePath = QDir::homeDirPath();
	}
	TextLabelPathName->setText( QDir::convertSeparators( m_BasePath ) );

	OBJECTVALUENODE_DECLARE_AND_GET(QString,file_name,conf);
	LineEditFileName->setText(VALUENODE_GET_VALUE(file_name));



}

FileWriteControl::~FileWriteControl()
{
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,buffer_max_length,conf);
	VALUENODE_SET_VALUE(buffer_max_length,SpinBoxBufferLength->maxValue());

	SIMPLEVALUENODE_DECLARE_AND_GET(Q_UINT32,buffer_length,conf);
	VALUENODE_SET_VALUE(buffer_length,SpinBoxBufferLength->value());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,save_bin,conf);
	VALUENODE_SET_VALUE(save_bin,CheckBoxBin->isChecked());

	SIMPLEVALUENODE_DECLARE_AND_GET(bool,save_wave,conf);
	VALUENODE_SET_VALUE(save_wave,CheckBoxWav->isChecked());

	OBJECTVALUENODE_DECLARE_AND_GET(QString,save_path,conf);
	VALUENODE_SET_VALUE(save_path,TextLabelPathName->text());

	OBJECTVALUENODE_DECLARE_AND_GET(QString,file_name,conf);
	VALUENODE_SET_VALUE(file_name,LineEditFileName->text());
}

void FileWriteControl::timerEvent( QTimerEvent *e )
{
	(void) e; // surpress warning
	if( m_isRunning )
		emit sendUpdate();
}

void FileWriteControl::setPause( bool b )
{
	if( b )
	{
		m_isRunning = false;
		emit sendCopy();
		GroupBoxSave->setEnabled( true );
	}
	else
	{
		GroupBoxSave->setEnabled( false );
		m_isRunning = true;
		TextLabelStatus->setText( "" );
	}
}

void FileWriteControl::setBufferFilling( int f )
{
	ProgressBarBufferFilling->setProgress( f );
}

void FileWriteControl::fireConfiguration()
{
	emit sendLength( SpinBoxBufferLength->value() );
}

void FileWriteControl::changeBufferLength( int len )
{
	emit sendLength( len );
}

void FileWriteControl::selectFolder()
{
	QString p;
	if( ! m_BasePath.isEmpty() )
		p = m_BasePath;

	QString s = Q3FileDialog::getExistingDirectory( p, this, NULL, "W�hle den Basisorder f�r die Daten" );
	if( ! s.isEmpty() )
	{
		m_BasePath = s;
		TextLabelPathName->setText( QDir::convertSeparators( m_BasePath ) );
	}
}

void FileWriteControl::saveData()
{
	QDir dir(m_BasePath);

	// Get file name and check if it is specified
	QString filename = LineEditFileName->text();
	if( filename.isEmpty() )
	{
		TextLabelStatus->setText("Dateiname angeben");
		return;
	}

	// Make sure that at least one file type is checked
	if( !CheckBoxBin->isChecked() && !CheckBoxWav->isChecked() )
	{
		TextLabelStatus->setText("Dateityp w�hlen");
		return;
	}

	// Make sure that the file name with ending .dat does not exist
	QFileInfo binfileinfo( dir, filename + ".bin" );
	if( binfileinfo.exists() )
	{
		TextLabelStatus->setText("Datei existiert bereits");
		return;
	}

	// Make sure that the file name with ending .wav does not exist
	QFileInfo wavfileinfo( dir, filename + ".wav" );
	if( wavfileinfo.exists() )
	{
		TextLabelStatus->setText("Datei existiert bereits");
		return;
	}

	// Check write permission
	if( CheckBoxBin->isChecked() )
	{
		QFile file( binfileinfo.absFilePath() );
		file.open( QIODevice::WriteOnly );
		file.close();
		if( !binfileinfo.isWritable() )
		{
			TextLabelStatus->setText("Kein Schreibberechtigung");
			return;
		}
	}
	else
	{
		QFile file( wavfileinfo.absFilePath() );
		file.open( QIODevice::WriteOnly );
		file.close();
		if( !wavfileinfo.isWritable() )
		{
			TextLabelStatus->setText("Kein Schreibberechtigung");
			return;
		}
	}

	// Save selected data
	if( CheckBoxBin->isChecked() )
	{
		emit sendDataSave( binfileinfo.absFilePath() );
	}
	if( CheckBoxWav->isChecked() )
	{
		emit sendWaveSave( wavfileinfo.absFilePath() );
	}

	// Show OK message
	TextLabelStatus->setText( "OK" );
}
