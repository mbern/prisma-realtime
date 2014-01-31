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

#ifndef DOCUMENT_NODE_H_
#define DOCUMENT_NODE_H_

#include "config/node.h"
#include "config/config_node.h"
//Added by qt3to4:
#include <Q3TextStream>

class DocumentNode : public Node
{
public:
	DocumentNode():Node("document"),content(NULL) {}
	virtual ~DocumentNode()
	{
		if(content)
			delete content;
	}
	virtual bool addNode(Node * n)
	{
		if(n && !content)
		{
			ConfigNode * nc = node_cast<ConfigNode>(n);
			if(nc)
			{
				content = nc;
				return TRUE;
			}
		}
		return FALSE;
	}
	virtual void save(Q3TextStream & s, NodeIndent & indent)
	{
		indent.resetIndent();
		// TODO: Write <!xml ... />
		if(content)
			content->save(s,indent);
	}
	virtual ConfigNode * getConfig()
	{
		return content;
	}
protected:
	ConfigNode * content;
};

#endif /*DOCUMENT_NODE_H_*/
