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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <q3textstream.h>
#include <qxml.h>
#include <q3ptrstack.h>
#include <qstring.h>

#define PRISMA_CONFIG_VERSION 2

class ConfigNode;
class DocumentNode;
class GlobalNode;
class Node;

class Configuration : public QXmlDefaultHandler
{
public:
	Configuration();
	virtual ~Configuration();

	virtual bool load(const QString & filename);
	virtual bool save(const QString & filename);

	virtual ConfigNode * getConfig();

protected:
	virtual Node * createNode(const QString& qName, const QXmlAttributes& atts);

	//QXmxContentHandler
	bool startDocument();
    bool endDocument();
    bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
    bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName );
    bool characters( const QString& ch );

	DocumentNode * root;
	GlobalNode * global;
	Q3PtrStack<Node> stack;
};

#endif // CONFIGURATION_H
