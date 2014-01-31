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

#ifndef PLUGINS_NODE_H_
#define PLUGINS_NODE_H_

#include "config/container_node.h"
#include "config/plugin_node.h"

class PluginsNode;

template<> inline PluginsNode * NodeCastImpl<PluginsNode>::node_cast(Node * n)
{
	if(n && n->getNodeName() == "plugins")
		return (PluginsNode*)n;
	return NULL;
}

class PluginsNode : public ContainerNode<PluginNode>
{
public:
	PluginsNode():ContainerNode<PluginNode>("plugins") {}
};

#endif /*PLUGINS_NODE_H_*/
