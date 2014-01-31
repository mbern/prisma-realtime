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

#ifndef NODE_MACRO_H_
#define NODE_MACRO_H_

#include "config/plugin_node.h"
#include "config/object_node.h"
#include "config/simplevalue_node.h"
#include "config/objectvalue_node.h"

#define NODE_VARIABLE_NAME(name) name##N##_
#define NODE_DECLARE(type,name) type * NODE_VARIABLE_NAME(name)
#define NODE_GET_AND_CAST(type,name,node) node_cast<type >(node->get(#name))

#define NODE_CREATE(nodetype,name) new nodetype(#name)
#define OBJECTNODE_CREATE(obj,name) new ObjectNode(#name,#obj)
#define VALUENODE_CREATE(nodetype,type,name) new nodetype<type>(#name,#type)

#define VALUENODE_GET_VALUE(name) NODE_VARIABLE_NAME(name)->getValue()
#define VALUENODE_SET_VALUE(name,value) NODE_VARIABLE_NAME(name)->setValue(value)

#define SIMPLEVALUENODE_DECLARE_AND_GET(type,name,node) \
	NODE_DECLARE(SimpleValueNode<type>,name) = NODE_GET_AND_CAST(SimpleValueNode<type>,name,node); \
	if(!NODE_VARIABLE_NAME(name)) { \
		NODE_VARIABLE_NAME(name) = VALUENODE_CREATE(SimpleValueNode,type,name); \
		node->addNode(NODE_VARIABLE_NAME(name)); }

#define OBJECTVALUENODE_DECLARE_AND_GET(type,name,node) \
	NODE_DECLARE(ObjectValueNode<type>,name) = NODE_GET_AND_CAST(ObjectValueNode<type>,name,node); \
	if(!NODE_VARIABLE_NAME(name)) { \
		NODE_VARIABLE_NAME(name) = VALUENODE_CREATE(ObjectValueNode,type,name); \
		node->addNode(NODE_VARIABLE_NAME(name)); }

#define PLUGINNODE_DECLARE_AND_GET(name,node) \
	NODE_DECLARE(PluginNode,name) = NODE_GET_AND_CAST(PluginNode,name,node); \
	if(!NODE_VARIABLE_NAME(name)) { \
		NODE_VARIABLE_NAME(name) = NODE_CREATE(PluginNode,name); \
		node->addNode(NODE_VARIABLE_NAME(name)); }

#define OBJECTNODE_DECLARE_AND_GET(name,obj,node) \
	NODE_DECLARE(ObjectNode,name) = NODE_GET_AND_CAST(ObjectNode,name,node); \
	if(!NODE_VARIABLE_NAME(name)) { \
		NODE_VARIABLE_NAME(name) = OBJECTNODE_CREATE(obj,name); \
		node->addNode(NODE_VARIABLE_NAME(name)); }

#endif /*NODE_MACRO_H_*/
