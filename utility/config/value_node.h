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

#ifndef VALUE_NODE_H_
#define VALUE_NODE_H_

#include "config/node.h"
//Added by qt3to4:
#include <Q3TextStream>

class ValueNode;

template<> inline ValueNode * NodeCastImpl<ValueNode>::node_cast(Node * n)
{
	if(n && n->getNodeName() == "value")
		return (ValueNode*)n;
	return NULL;
}

class ValueNode : public Node
{
public:
	ValueNode(const QString & id, const QString & type):Node("value"),id(id),type(type) {}
	virtual QString getValueStr() = 0;
	virtual void save(Q3TextStream & s, NodeIndent & indent)
	{
		const QString & nodename = this->getNodeName();
		const QString & valuestr = this->getValueStr();

		s << indent << "<" << nodename << " id=\"" << id << "\" type=\"" << type << "\"";

		if(this->hasAttributes())
			s << " " << this->getAttrStr();

		if(!valuestr.isEmpty())
		{
			s << ">" << valuestr << "</" << nodename << ">\n";
		}
		else
		{
			s << "/>\n";
		}
	}
	virtual const QString & getId() const
	{
		return id;
	}
	virtual const QString & getType() const
	{
		return type;
	}
protected:
	QString id;
	QString type;
};

#endif /*VALUE_NODE_H_*/
