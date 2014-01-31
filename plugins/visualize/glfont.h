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

#ifndef GLFONT_H
#define GLFONT_H

#include <qstring.h>
#include <qxml.h>
#include <qmap.h>
#include <qmutex.h>
#include <qpoint.h>

#include <GL/glew.h>

class GLFont
{
protected:
	struct FontInfo {
		QString face;		//The font face used to generate the font.
		unsigned short size;	//The size of the font.
		bool bold;			//Whether the font is bold.
		bool italic;		//Whether the font is italic.
		QString charset;	//The character set used.
	};

	struct FontCharDescriptor {

		unsigned short x, y;			//The position of the character image in the texture.
		unsigned short width, height;	//The size of the character image in the texture.
		unsigned short xoffset, yoffset;//The offset from the cursor position where the character image is to be drawn.
		unsigned short xadvance;		//The horizontal advancement of the cursor for the next character.
		unsigned short page;			//The texture where the character image is located.

		FontCharDescriptor() : x(0), y(0), width(0), height(0), xoffset(0), yoffset(0), xadvance(0), page(0) { }
	};

	struct FontCharset {
		unsigned short lineHeight;		//The vertical distance between two text lines.
		unsigned short base;			//The vertical distance from the top of the text line to the common base of the characters.
		unsigned short scaleW, scaleH;	//The size of the textures.
		unsigned short pages;			//The number of textures needed to fit all characters.
		FontCharDescriptor chars[256];
	};

	struct Interleaved_T2F_V3F {
		GLfloat tu;
		GLfloat tv;
		GLfloat x;
		GLfloat y;
		GLfloat z;
	};

	struct TextureChar {
		Interleaved_T2F_V3F edges[4];
		GLfloat set( const FontCharset & fc, unsigned char c, GLfloat curx, GLfloat cury );
	};

	struct GLFontInfo {
		QString filename;
		int isInitialized;
		GLuint ibuffer;
		FontInfo fontinfo;
		FontCharset fontcharset;

		GLFontInfo() : isInitialized(0) {}
	};

public:
	static GLFont * createGLFont( const QString &filename, unsigned int buffersize);
	~GLFont();

	void reset();
	void drawText();
	unsigned int addTextTop( const QString & text, const QPoint & point );
	unsigned int addTextBase( const QString & text, const QPoint & point );
	unsigned int addTextBottom( const QString & text, const QPoint & point );
	unsigned int stringLength( const QString & text );
	unsigned int charHeight( char c );
	void init();
protected:
	GLFont( GLFontInfo & info, unsigned int buffersize );
	void loadFontDescription();
	void loadFontTextmap();
	unsigned int addText( const QString & text, const QPoint & point );
protected:


	class XMLHandler : public QXmlDefaultHandler
	{
	public:
		virtual bool startDocument() { return true; }
		virtual bool endDocument() { return true; }
		virtual bool startElement( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
		virtual bool endElement( const QString & namespaceURI, const QString & localName, const QString & qName ) ;
		XMLHandler( GLFontInfo & info ): m_info(info) {}
	private:
		GLFontInfo & m_info;
	};

	GLuint m_VBO;
	unsigned int m_nChars;
	unsigned int m_maxChars;

	GLFontInfo & m_pGLFontInfo;

	static QMap<QString,GLFontInfo> sm_fonts;
	static QMutex sm_mutex;
};

#endif // GLFONT_H
