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

#ifndef CONFIG_NODE_H_
#define CONFIG_NODE_H_

#include "config/container_node.h"
#include "config/global_node.h"
#include "config/application_node.h"
#include "config/plugins_node.h"

class ConfigNode;

template<> inline ConfigNode * NodeCastImpl<ConfigNode>::node_cast(Node * n)
{
	if(n && n->getNodeName() == "prismaconfig")
		return (ConfigNode*)n;
	return NULL;
}

class ConfigNode : public ContainerNode<Node>
{
public:
	ConfigNode(const QString & ver, const QString & desc):ContainerNode<Node>("prismaconfig"),version(ver),description(desc),global(NULL),app(NULL),plugins(NULL) {}
	virtual bool addNode(Node * n)
	{
		if(n)
		{
			if(!global)
			{
				GlobalNode * nc = node_cast<GlobalNode>(n);
				if(nc)
				{
					global = nc;
					return ContainerNode<Node>::addNode(n);
				}
			}
			else if(global && !app)
			{
				ApplicationNode * nc = node_cast<ApplicationNode>(n);
				if(nc)
				{
					app = nc;
					return ContainerNode<Node>::addNode(n);
				}
			}
			else if(global && app && !plugins)
			{
				PluginsNode * nc = node_cast<PluginsNode>(n);
				if(nc)
				{
					plugins = nc;
					return ContainerNode<Node>::addNode(n);
				}
			}
		}
		return FALSE;
	}
	virtual bool hasAttributes()
	{
		return TRUE;
	}
	virtual QString getAttrStr()
	{
		return QString("version=\"") + version + QString("\" description=\"") + description + QString("\"");
	}
	virtual GlobalNode * getGlobalNode()
	{
		return global;
	}
	virtual ApplicationNode * getApplicationNode()
	{
		return app;
	}
	virtual PluginsNode * getPluginsNode()
	{
		return plugins;
	}
	virtual const QString & getDescription() const
	{
		return description;
	}
protected:
	const QString version;
	const QString description;
	GlobalNode * global;
	ApplicationNode * app;
	PluginsNode * plugins;
};

#endif /*CONFIG_NODE_H_*/
