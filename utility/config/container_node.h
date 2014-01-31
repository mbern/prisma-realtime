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

#ifndef CONTAINER_NODE_H_
#define CONTAINER_NODE_H_

#include "config/node.h"

#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3TextStream>

template<class N>
class ContainerNode : public Node
{
public:
	ContainerNode(const QString & name):Node(name)
	{
		childs.setAutoDelete(TRUE);
	}
	virtual bool addNode(Node * n)
	{
		if(n)
		{
			N * nc = node_cast<N>(n);
			if(nc)
			{
				childs.append(nc);
				return TRUE;
			}
		}
		return FALSE;
	}
	virtual void save(Q3TextStream & s, NodeIndent & indent)
	{
		s << indent << "<" << this->getNodeName();
		if(this->hasAttributes())
			s << " " << this->getAttrStr();
		s << ">\n";

		++indent;
		for(N * n = childs.first(); n; n = childs.next())
			n->save(s,indent);
		--indent;

		s << indent << "</" << this->getNodeName() << ">\n";
	}
	virtual N * get(const QString & id)
	{
		for(N * n = childs.first(); n; n = childs.next())
		{
			if(n->getId() == id)
				return n;
		}
		return NULL;
	}
protected:
	Q3PtrList<N> childs;
};

#endif /*CONTAINER_NODE_H_*/
