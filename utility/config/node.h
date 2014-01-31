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

#ifndef NODE_H
#define NODE_H

#include <qstring.h>
#include <q3textstream.h>

class Node;
template<class T> struct NodeCastImpl;

template<class T>
inline T * node_cast(Node * n)
{
	return NodeCastImpl<T>::node_cast(n);
}

template<class T>
struct NodeCastImpl
{
	static T * node_cast(Node * n);
};

template<> inline Node * NodeCastImpl<Node>::node_cast(Node * n)
{
	return n;
}

class NodeIndent : public QString
{
public:
	NodeIndent():indent_num(0) {}
	NodeIndent & operator++()
	{
		indent_num += 4;
		fill(' ',indent_num);
		return *this;
	}
	NodeIndent & operator--()
	{
		indent_num -= 4;
		fill(' ',indent_num);
		return *this;
	}
	void resetIndent()
	{
		indent_num = 0;
		fill(' ',indent_num);
		//indent = QString::null;
	}
//	virtual QString Indent()
//	{
//		return indent;
//	}
private:
	Q_UINT32 indent_num;
};

class Node
{
public:
	Node(const QString & name):node_name(name), null_string(QString::null) {}
	virtual ~Node() {}

	virtual bool addNode(Node * n)
	{
		(void) n; //surpress warning
		return FALSE;
	}
	virtual bool addData(const QString & d)
	{
		(void) d; //surpress warning
		return FALSE;
	}
	virtual QString getNodeName()
	{
		return node_name;
	}
	virtual bool hasAttributes()
	{
		return FALSE;
	}
	virtual QString getAttrStr()
	{
		return QString::null;
	}
	virtual void save(Q3TextStream & s, NodeIndent & indent) = 0;
	virtual const QString & getId() const
	{
		return null_string;
	}
private:
	QString node_name;
	QString null_string;
};

#endif /*NODE_H*/
