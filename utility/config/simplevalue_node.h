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

#ifndef SIMPLEVALUE_NODE_H_
#define SIMPLEVALUE_NODE_H_

#include "config/value_node.h"

template<class T> class SimpleValueNode;

template<> inline SimpleValueNode<Q_UINT8> * NodeCastImpl<SimpleValueNode<Q_UINT8> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "Q_UINT8")
		return (SimpleValueNode<Q_UINT8>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<Q_UINT16> * NodeCastImpl<SimpleValueNode<Q_UINT16> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "Q_UINT16")
		return (SimpleValueNode<Q_UINT16>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<Q_UINT32> * NodeCastImpl<SimpleValueNode<Q_UINT32> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "Q_UINT32")
		return (SimpleValueNode<Q_UINT32>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<Q_INT8> * NodeCastImpl<SimpleValueNode<Q_INT8> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "Q_INT8")
		return (SimpleValueNode<Q_INT8>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<Q_INT16> * NodeCastImpl<SimpleValueNode<Q_INT16> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "Q_INT16")
		return (SimpleValueNode<Q_INT16>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<Q_INT32> * NodeCastImpl<SimpleValueNode<Q_INT32> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "Q_INT32")
		return (SimpleValueNode<Q_INT32>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<bool> * NodeCastImpl<SimpleValueNode<bool> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "bool")
		return (SimpleValueNode<bool>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<double> * NodeCastImpl<SimpleValueNode<double> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "double")
		return (SimpleValueNode<double>*)vn;
	return NULL;
}

template<> inline SimpleValueNode<float> * NodeCastImpl<SimpleValueNode<float> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "float")
		return (SimpleValueNode<float>*)vn;
	return NULL;
}

template< class T >
class SimpleValueNode: public ValueNode
{
public:
	SimpleValueNode(const QString & id, const QString & type):ValueNode(id,type),value(0) {}
	virtual bool addData(const QString & d);
	virtual QString getValueStr()
	{
		return QString::number(value);
	}
	virtual T getValue() const
	{
		return value;
	}
	virtual void setValue(T v)
	{
		value = v;
	}
protected:
	T value;
};

// Specialisation of addData()
template <> inline bool SimpleValueNode<double>::addData(const QString & d)
{
	bool ok = FALSE;
	value = d.toDouble(&ok);
	return ok;
}

template <> inline bool SimpleValueNode<float>::addData(const QString & d)
{
	bool ok = FALSE;
	value = d.toFloat(&ok);
	return ok;
}

template <> inline bool SimpleValueNode<Q_INT8>::addData(const QString & d)
{
	bool ok = FALSE;
	Q_INT16 v = d.toShort(&ok);
	if(ok && v == (Q_INT16)((Q_INT8)v))
		value = (Q_INT8)v;
	return ok;
}

template <> inline bool SimpleValueNode<Q_UINT8>::addData(const QString & d)
{
	bool ok = FALSE;
	Q_UINT16 v = d.toShort(&ok);
	if(ok && v == (Q_UINT16)((Q_UINT8)v))
		value = (Q_UINT8)v;
	return ok;
}

template <> inline bool SimpleValueNode<Q_INT16>::addData(const QString & d)
{
	bool ok = FALSE;
	value = d.toShort(&ok);
	return ok;
}

template <> inline bool SimpleValueNode<Q_UINT16>::addData(const QString & d)
{
	bool ok = FALSE;
	value = d.toUShort(&ok);
	return ok;
}

template <> inline bool SimpleValueNode<Q_INT32>::addData(const QString & d)
{
	bool ok = FALSE;
	value = d.toLong(&ok);
	return ok;
}

template <> inline bool SimpleValueNode<Q_UINT32>::addData(const QString & d)
{
	bool ok = FALSE;
	value = d.toULong(&ok);
	return ok;
}

template <> inline bool SimpleValueNode<bool>::addData(const QString & d)
{
	bool ok = FALSE;

	if(d == "TRUE" || d == "1")
	{
		value = TRUE;
		ok = TRUE;
	}
	else if(d == "FALSE" || d == "0")
	{
		value = FALSE;
		ok = TRUE;
	}

	return ok;
}

// Specialisation of getValueStr()
template <> inline QString SimpleValueNode<bool>::getValueStr()
{
	if(value)
		return QString("TRUE");
	else
		return QString("FALSE");
}

#endif /*SIMPLEVALUE_NODE_H_*/
