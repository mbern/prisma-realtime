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

#ifndef OBJECTVALUE_NODE_H_
#define OBJECTVALUE_NODE_H_

#include "config/value_node.h"

#include <qstringlist.h>

// TODO: remove this because it has to be defined by the user
#include <qsize.h>
#include <qpoint.h>
#include <qrect.h>

template<class T> class ObjectValueNode;

template<> inline ObjectValueNode<QSize> * NodeCastImpl<ObjectValueNode<QSize> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "QSize")
		return (ObjectValueNode<QSize>*)vn;
	return NULL;
}

template<> inline ObjectValueNode<QPoint> * NodeCastImpl<ObjectValueNode<QPoint> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "QPoint")
		return (ObjectValueNode<QPoint>*)vn;
	return NULL;
}

template<> inline ObjectValueNode<QRect> * NodeCastImpl<ObjectValueNode<QRect> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "QRect")
		return (ObjectValueNode<QRect>*)vn;
	return NULL;
}

template<> inline ObjectValueNode<QString> * NodeCastImpl<ObjectValueNode<QString> >::node_cast(Node * n)
{
	ValueNode * vn = ::node_cast<ValueNode>(n);
	if(vn && vn->getType() == "QString")
		return (ObjectValueNode<QString>*)vn;
	return NULL;
}

template<class T>
class ObjectValueNode : public ValueNode
{
public:
	ObjectValueNode(const QString & id, const QString & type):ValueNode(id,type) {}
	virtual bool addData(const QString & d);
	virtual QString getValueStr();
	virtual const T & getValue() const
	{
		return object;
	}
	virtual void setValue(const T & v)
	{
		object = v;
	}
protected:
	T object;
};

// Specialisation of addData()
template <> inline bool ObjectValueNode<QPoint>::addData(const QString & d)
{
	QStringList values = QStringList::split(',',d);

	if(values.size() != 2)
		return FALSE;

	bool ok = FALSE;

	object.setX(values[0].toInt(&ok));
	if(!ok)
		return FALSE;

	object.setY(values[1].toInt(&ok));
	if(!ok)
		return FALSE;

	return TRUE;
}

template <> inline bool ObjectValueNode<QSize>::addData(const QString & d)
{
	QStringList values = QStringList::split(',',d);

	if(values.size() != 2)
		return FALSE;

	bool ok = FALSE;

	object.setWidth(values[0].toInt(&ok));
	if(!ok)
		return FALSE;

	object.setHeight(values[1].toInt(&ok));
	if(!ok)
		return FALSE;

	return TRUE;
}

template <> inline bool ObjectValueNode<QRect>::addData(const QString & d)
{
	QStringList values = QStringList::split(',',d);

	if(values.size() != 4)
		return FALSE;

	bool ok = FALSE;

	object.setX(values[0].toInt(&ok));
	if(!ok)
		return FALSE;

	object.setY(values[1].toInt(&ok));
	if(!ok)
		return FALSE;

	object.setWidth(values[2].toInt(&ok));
	if(!ok)
		return FALSE;

	object.setHeight(values[3].toInt(&ok));
	if(!ok)
		return FALSE;

	return TRUE;
}

template <> inline bool ObjectValueNode<QString>::addData(const QString & d)
{
	object = d;
	return TRUE;
}

// Specialisation of getValueStr()
template <> inline QString ObjectValueNode<QPoint>::getValueStr()
{
	return QString::number(object.x()) + "," + QString::number(object.y());
}

template <> inline QString ObjectValueNode<QSize>::getValueStr()
{
	return QString::number(object.width()) + "," + QString::number(object.height());
}

template <> inline QString ObjectValueNode<QRect>::getValueStr()
{
	return QString::number(object.x()) + "," + QString::number(object.y()) + "," + QString::number(object.width()) + "," + QString::number(object.height());
}

template <> inline QString ObjectValueNode<QString>::getValueStr()
{
	return object;
}

#endif /*OBJECTVALUE_NODE_H_*/
