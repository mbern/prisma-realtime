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

#include "configuration.h"

#include "config/config_node.h"
#include "config/global_node.h"
#include "config/application_node.h"
#include "config/object_node.h"
#include "config/plugin_node.h"
#include "config/plugins_node.h"
#include "config/document_node.h"
#include "config/simplevalue_node.h"
#include "config/objectvalue_node.h"
#include "config/proxyvalue_node.h"

#include <qfile.h>
//Added by qt3to4:
#include <Q3TextStream>

Configuration::Configuration(): root(NULL), global(NULL)
{

}

Configuration::~Configuration()
{
	if(root)
		delete root;
}

bool Configuration::load(const QString & filename)
{
	QFile f(filename);
	if(f.open(QIODevice::ReadOnly))
	{
		QXmlInputSource xmlsoure(&f);
		QXmlSimpleReader xmlreader;
		xmlreader.setContentHandler(this);
		xmlreader.setDTDHandler(this);
		xmlreader.setDeclHandler(this);
		xmlreader.setEntityResolver(this);
		xmlreader.setErrorHandler(this);
		xmlreader.setLexicalHandler(this);
		bool ret = xmlreader.parse(xmlsoure);
		f.close();
		return ret;
	}
	else
	{
		qWarning("Could not open file: %s for reading", filename.ascii());
		return false;
	}
}

bool Configuration::save(const QString & filename)
{
	QFile f(filename);
	if(f.open(QIODevice::WriteOnly))
	{
		Q3TextStream textstream(&f);
		NodeIndent indent;
		root->save(textstream,indent);
		f.close();
		return true;
	}
	else
	{
		qWarning("Could not open file %s for writing", filename.ascii());
		return false;
	}
}

ConfigNode * Configuration::getConfig()
{
	ConfigNode * n = NULL;
	if(root)
	{
		n = root->getConfig();
	}
	return n;
}

Node * Configuration::createNode(const QString& qName, const QXmlAttributes& atts)
{
	QString v_expected;

	if(qName == "prismaconfig")
	{
		const QString & version = atts.value("version");
		// Check for version
		v_expected.setNum((int)PRISMA_CONFIG_VERSION);
		if(version != v_expected)
		{
			qWarning("File version (%s) does not match expected version (%s)",version.ascii(),v_expected.ascii());
			return NULL;
		}

		const QString & description = atts.value("description");
		if(description.isEmpty())
		{
			qWarning("No description for this file available");
			return NULL;
		}

		// Create node
		return new ConfigNode(version, description);
	}
	else if(qName == "global")
	{
		global = new GlobalNode();
		return global;
	}
	else if(qName == "application")
	{
		return new ApplicationNode();
	}
	else if(qName == "plugins")
	{
		return new PluginsNode();
	}
	else if(qName == "plugin")
	{
		return new PluginNode(atts.value("id"));
	}
	else if(qName == "object")
	{
		return new ObjectNode(atts.value("id"),atts.value("class"));
	}
	else if(qName == "value")
	{
		const QString & id = atts.value("id");
		const QString & type = atts.value("type");
		if(type == "double")
		{
			return new SimpleValueNode<double>(id,type);
		}
		else if(type == "float")
		{
			return new SimpleValueNode<float>(id,type);
		}
		else if(type == "Q_INT8")
		{
			return new SimpleValueNode<Q_INT8>(id,type);
		}
		else if(type == "Q_INT16")
		{
			return new SimpleValueNode<Q_INT16>(id,type);
		}
		else if(type == "Q_INT32")
		{
			return new SimpleValueNode<Q_INT32>(id,type);
		}
		else if(type == "Q_UINT8")
		{
			return new SimpleValueNode<Q_UINT8>(id,type);
		}
		else if(type == "Q_UINT16")
		{
			return new SimpleValueNode<Q_UINT16>(id,type);
		}
		else if(type == "Q_UINT32")
		{
			return new SimpleValueNode<Q_UINT32>(id,type);
		}
		else if(type == "bool")
		{
			return new SimpleValueNode<bool>(id,type);
		}
		else if(type == "PROXY")
		{
			const QString & refid = atts.value("refid");
			if(global)
			{
				ValueNode * refnode = global->get(refid);
				if(refnode)
				{
					QString refnodetype = refnode->getType();
					if(refnodetype == "Q_UINT8")
					{
						return new ProxyValueNode<SimpleValueNode<Q_UINT8> >(id,node_cast<SimpleValueNode<Q_UINT8> >(refnode));
					}
					else if(refnodetype == "Q_UINT16")
					{
						return new ProxyValueNode<SimpleValueNode<Q_UINT16> >(id,node_cast<SimpleValueNode<Q_UINT16> >(refnode));
					}
					else if(refnodetype == "Q_UINT32")
					{
						return new ProxyValueNode<SimpleValueNode<Q_UINT32> >(id,node_cast<SimpleValueNode<Q_UINT32> >(refnode));
					}
					else if(refnodetype == "Q_INT8")
					{
						return new ProxyValueNode<SimpleValueNode<Q_INT8> >(id,node_cast<SimpleValueNode<Q_INT8> >(refnode));
					}
					else if(refnodetype == "Q_INT16")
					{
						return new ProxyValueNode<SimpleValueNode<Q_INT16> >(id,node_cast<SimpleValueNode<Q_INT16> >(refnode));
					}
					else if(refnodetype == "Q_INT32")
					{
						return new ProxyValueNode<SimpleValueNode<Q_INT32> >(id,node_cast<SimpleValueNode<Q_INT32> >(refnode));
					}
					else if(refnodetype == "float")
					{
						return new ProxyValueNode<SimpleValueNode<float> >(id,node_cast<SimpleValueNode<float> >(refnode));
					}
					else if(refnodetype == "double")
					{
						return new ProxyValueNode<SimpleValueNode<double> >(id,node_cast<SimpleValueNode<double> >(refnode));
					}
				}
			}
		}
		else if(type == "QPoint")
		{
			return new ObjectValueNode<QPoint>(id,type);
		}
		else if(type == "QSize")
		{
			return new ObjectValueNode<QSize>(id,type);
		}
		else if(type == "QString")
		{
			return new ObjectValueNode<QString>(id,type);
		}
		else if(type == "QRect")
		{
			return new ObjectValueNode<QRect>(id,type);
		}
	}
	return NULL;
}

bool Configuration::startDocument()
{
	root = new DocumentNode();
	stack.push(root);
	return TRUE;
}

bool Configuration::endDocument()
{
	bool ok = FALSE;
	Node * n = stack.pop();
	if(n)
	{
		if(n->getNodeName() == "document")
		{
			if(stack.isEmpty())
				ok = TRUE;
		}
	}

	if(!ok)
	{
		qWarning("Failed: endDocument");
	}

	return ok;
}

bool Configuration::startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts )
{
	(void) namespaceURI; //surpress warning
	(void) localName; //surpress warning

	bool ok = FALSE;

	Node * n = createNode(qName,atts);
	if(n)
	{
		if(stack.current()->addNode(n))
		{
			stack.push(n);
			ok = TRUE;
		}
	}

	if(!ok)
	{
		qWarning("Parse failed: startElement qName=%s",qName.ascii());
	}

	return ok;
}

bool Configuration::endElement( const QString& namespaceURI, const QString& localName, const QString& qName )
{
	(void) namespaceURI; //surpress warning
	(void) localName; //surpress warning
	(void) qName; //surpress warning

	bool ok = FALSE;

	Node * n = stack.pop();
	if(n)
	{
		if(n->getNodeName() == qName)
			ok = TRUE;
	}

	if(!ok)
	{
		qWarning("Parse failed: endElement qName=%s",qName.ascii());
	}

	return ok;
}

bool Configuration::characters( const QString& ch )
{
	bool ok = FALSE;

	QString c = ch.stripWhiteSpace();

	if(!c.isEmpty())
	{
		Node * n = stack.current();
		if(n)
		{
			ok = n->addData(c);
		}
	}
	else
		ok = TRUE;

	if(!ok)
	{
		qWarning("Parse failed: characters ch='%s'",ch.ascii());
	}

	return ok;
}
