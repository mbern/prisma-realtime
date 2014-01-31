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

#ifndef OBJECT_NODE_H_
#define OBJECT_NODE_H_

#include "config/container_node.h"
#include "config/value_node.h"

class ObjectNode;

template<> inline ObjectNode * NodeCastImpl<ObjectNode>::node_cast(Node * n)
{
	if(n && n->getNodeName() == "object")
		return (ObjectNode*)n;
	return NULL;
}

class ObjectNode : public ContainerNode<ValueNode>
{
public:
	ObjectNode(const QString & id, const QString & cn):ContainerNode<ValueNode>("object"),id(id),classname(cn) {}
	virtual bool hasAttributes()
	{
		return TRUE;
	}
	virtual QString getAttrStr()
	{
		return QString("id=\"") + id + QString("\" class=\"") + classname + QString("\"");
	}
	virtual const QString & getId() const
	{
		return id;
	}
protected:
	QString id;
	QString classname;
};

#endif /*OBJECT_NODE_H_*/
