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

//Added by qt3to4:
#include <Q3TextStream>
#ifndef PROXYVALUE_NODE_H_
#define PROXYVALUE_NODE_H_

template<class T>
class ProxyValueNode: public T
{
public:
	ProxyValueNode(const QString & id, T * node):T(id,node->getType()),obj(node)
	{
		if(obj)
			this->setValue(obj->getValue());
	}
	virtual ~ProxyValueNode()
	{
	}
	virtual bool addData(const QString & d)
	{
		if(d.isEmpty())
			return TRUE;
		return FALSE;
	}
	virtual QString getValueStr()
	{
		return QString::null;
	}
	virtual void save(Q3TextStream & s, NodeIndent & indent)
	{
		s << indent << "<value id=\"" << this->getId() << "\" type=\"PROXY\" ";
		s << this->getAttrStr();
		s << "/>\n";
	}
	virtual bool hasAttributes()
	{
		if(obj)
			return TRUE;
		return FALSE;
	}
	virtual QString getAttrStr()
	{
		if(obj)
			return QString("refid=\"") + obj->getId() + QString("\"");
		return QString::null;
	}
protected:
	T * obj;
};

#endif /*PROXYVALUE_NODE_H_*/
