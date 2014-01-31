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

#include "glfont.h"

#include <qimage.h>
#include <qmessagebox.h>

QMap<QString,GLFont::GLFontInfo> GLFont::sm_fonts;
QMutex GLFont::sm_mutex;

GLFont::GLFont( GLFontInfo & info, unsigned int buffersize )
: m_pGLFontInfo(info)
{
	m_maxChars = buffersize;
	m_nChars = 0;
	m_VBO = 0;
}

GLFont::~GLFont()
{
}

GLFont * GLFont::createGLFont( const QString &filename, unsigned int buffersize)
{
	QMutexLocker l(&sm_mutex);
	GLFontInfo & info = sm_fonts[filename];
	if(info.filename != filename)
		info.filename = filename;
	return new GLFont(info,buffersize);
}

void GLFont::reset()
{
	m_nChars = 0;
}

void GLFont::drawText()
{
	glPushAttrib( GL_ENABLE_BIT );
	{
		glEnable( GL_TEXTURE_2D );
		glDisable( GL_DEPTH_TEST );

		//glColor3d(1,1,1);
		glBindTexture( GL_TEXTURE_2D, m_pGLFontInfo.ibuffer );
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO );
		glInterleavedArrays( GL_T2F_V3F, 0, (char*)NULL );
		glDrawArrays( GL_QUADS, 0, m_nChars*4 );
	}
	glPopAttrib();
}

unsigned int GLFont::addTextTop( const QString & text, const QPoint & point )
{
	return addText( text, point );
}

unsigned int GLFont::addTextBase( const QString & text, const QPoint & point )
{
	return addText( text, QPoint(0,m_pGLFontInfo.fontcharset.base) + point );
}

unsigned int GLFont::addTextBottom( const QString & text, const QPoint & point )
{
	return addText( text, QPoint(0,m_pGLFontInfo.fontcharset.lineHeight) + point );
}

unsigned int GLFont::addText( const QString & text, const QPoint & point )
{
	unsigned int nsave = m_nChars;
	unsigned int i = 0;

	// Bind VBO
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO );
	TextureChar * buffer = (TextureChar*)glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );

	if(buffer == NULL)
		return 0;

	// Set buffer pointer to the current position
	buffer += m_nChars;

	GLfloat curx = point.x();
	GLfloat cury = point.y();
	for( i = 0; i < text.length() && m_nChars < m_maxChars  ; i++ )
	{
		curx = buffer->set( m_pGLFontInfo.fontcharset, text.at(i).ascii(), curx, cury );
		buffer++;
		m_nChars++;
	}

	if( !glUnmapBufferARB( GL_ARRAY_BUFFER_ARB ) )
	{
		m_nChars = nsave;
		i = 0;
	}

	return i;
}

unsigned int GLFont::stringLength( const QString & text )
{
	unsigned int strlen = 0;
	for( unsigned int i = 0; i < text.length(); i++ )
	{
		strlen += m_pGLFontInfo.fontcharset.chars[text.at(i).ascii()].xadvance;
	}
	return strlen;
}

unsigned int GLFont::charHeight( char c )
{
	return m_pGLFontInfo.fontcharset.chars[(int)c].height;
}

void GLFont::init()
{
	QStringList fneeded;
	QStringList funsupported ;

	glGenBuffersARB( 1, &m_VBO );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, m_maxChars*sizeof(TextureChar), NULL, GL_DYNAMIC_DRAW_ARB );

	// Prevent concurrent access
	sm_mutex.lock();
	{
		// Process only if not initialized
		if(m_pGLFontInfo.isInitialized == 0)
		{
			loadFontDescription();
			loadFontTextmap();

			// Mark font environment initalized
			m_pGLFontInfo.isInitialized = 1;
		}
	}
	sm_mutex.unlock();

}

void GLFont::loadFontDescription()
{
	// Create a xml reader
	QXmlSimpleReader reader;
	// Create the xml handler
	XMLHandler handler( m_pGLFontInfo );

	// Register the handler in the reader
	reader.setContentHandler( &handler );

	// Open font description file
	QFile file( m_pGLFontInfo.filename );
	if( file.open( QIODevice::ReadOnly ) )
	{
		// Create xml input source
		QXmlInputSource xmlsource((QIODevice*)&file);

		// Parse the font description file
		reader.parse( xmlsource );

		// Close the file
		file.close();
	}
	else
		qFatal("Could not open font decription file %s",m_pGLFontInfo.filename.ascii());
}

void GLFont::loadFontTextmap()
{
	QImage img( m_pGLFontInfo.filename.replace( m_pGLFontInfo.filename.length()-3,3, QString("png") ) );

	glGenTextures( 1, &m_pGLFontInfo.ibuffer );
	glBindTexture( GL_TEXTURE_2D, m_pGLFontInfo.ibuffer );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, img.width(), img.height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, img.bits() );
}

bool GLFont::XMLHandler::startElement( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts )
{
	(void) localName; // to surpress warning
	(void) namespaceURI; // to surpress warning

	if( qName == QString("font") )
	{
	}
	else if( qName == QString("info") )
	{
		m_info.fontinfo.face = atts.value( QString( "face" ) );
		m_info.fontinfo.size = atts.value( QString( "size" ) ).toUShort();
		m_info.fontinfo.bold = atts.value( QString( "bold" ) ).toInt();
		m_info.fontinfo.italic = atts.value( QString( "italic" ) ).toInt();
		m_info.fontinfo.charset = atts.value( QString( "charset" ) );
	}
	else if( qName == QString("common") )
	{
		m_info.fontcharset.lineHeight = atts.value( QString( "lineHeight" ) ).toUShort();
		m_info.fontcharset.base = atts.value( QString( "base" ) ).toUShort();
		m_info.fontcharset.scaleW = atts.value( QString( "scaleW" ) ).toUShort();
		m_info.fontcharset.scaleH = atts.value( QString( "scaleH" ) ).toUShort();
		m_info.fontcharset.pages = atts.value( QString( "pages" ) ).toUShort();
	}
	else if( qName == QString("char") )
	{
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].x = atts.value( QString( "x" ) ).toUShort();
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].y = atts.value( QString( "y" ) ).toUShort();
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].width = atts.value( QString( "width" ) ).toUShort();
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].height = atts.value( QString( "height" ) ).toUShort();
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].xoffset = atts.value( QString( "xoffset" ) ).toUShort();
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].yoffset = atts.value( QString( "yoffset" ) ).toUShort();
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].xadvance = atts.value( QString( "xadvance" ) ).toUShort();
		m_info.fontcharset.chars[ atts.value( QString( "id" ) ).toUShort() ].page = atts.value( QString( "page" ) ).toUShort();
	}
	else if( qName == QString("kerning") )
	{
	}
	else
	{
	}

	return true;
}

bool GLFont::XMLHandler::endElement( const QString & namespaceURI, const QString & localName, const QString & qName )
{
	(void) namespaceURI; // to surpress warning
	(void) localName; // to surpress warning
	(void) qName; // to surpress warning
	return true;
}

GLfloat GLFont::TextureChar::set( const FontCharset & fc, unsigned char c, GLfloat curx, GLfloat cury )
{
	unsigned int CharX = fc.chars[c].x;
	unsigned int CharY = fc.chars[c].y;
	unsigned int Width = fc.chars[c].width;
	unsigned int Height = fc.chars[c].height;
	unsigned int OffsetX = fc.chars[c].xoffset;
	unsigned int OffsetY = fc.chars[c].yoffset;

	//upper left
	edges[0].tu = (float) CharX / (float) fc.scaleW;
	edges[0].tv = (float) CharY / (float) fc.scaleH;
	edges[0].x = (float) (curx + OffsetX);
	edges[0].y = (float) cury - OffsetY;
	edges[0].z = 0;

	//upper right
	edges[1].tu = (float) (CharX+Width) / (float) fc.scaleW;
	edges[1].tv = (float) CharY / (float) fc.scaleH;
	edges[1].x = (float) (Width + curx + OffsetX);
	edges[1].y = (float) cury - OffsetY;
	edges[1].z = 0;

	//lower right
	edges[2].tu = (float) (CharX+Width) / (float) fc.scaleW;
	edges[2].tv = (float) (CharY+Height) / (float) fc.scaleH;
	edges[2].x = (float) (Width + curx + OffsetX);
	edges[2].y = (float) cury - OffsetY - Height;
	edges[2].z = 0;

	//lower left
	edges[3].tu = (float) CharX / (float) fc.scaleW;
	edges[3].tv = (float) (CharY+Height) / (float) fc.scaleH;
	edges[3].x = (float) (curx + OffsetX);
	edges[3].y = (float) cury - OffsetY - Height;
	edges[3].z = 0;

	return curx + fc.chars[c].xadvance;
}

