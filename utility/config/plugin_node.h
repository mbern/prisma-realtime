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

#ifndef PLUGIN_NODE_H_
#define PLUGIN_NODE_H_

#include "config/container_node.h"
#include "config/object_node.h"

class PluginNode;

template<> inline PluginNode * NodeCastImpl<PluginNode>::node_cast(Node * n)
{
	if(n && n->getNodeName() == "plugin")
		return (PluginNode*)n;
	return NULL;
}

class PluginNode : public ContainerNode<ObjectNode>
{
public:
	PluginNode(const QString & id):ContainerNode<ObjectNode>("plugin"),id(id) {}
	virtual bool hasAttributes()
	{
		return TRUE;
	}
	virtual QString getAttrStr()
	{
		return QString("id=\"") + id + QString("\"");
	}
	virtual const QString & getId() const
	{
		return id;
	}
protected:
	QString id;
};

#endif /*PLUGIN_NODE_H_*/
