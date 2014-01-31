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

#ifndef SIMPLE_GRAPH_H
#define SIMPLE_GRAPH_H

#include "plugin_global.h"
#include "prisma_defines.h"

#include "pi_configuration.h"
#include "pi_unknown.h"
#include "pi_guicontainer.h"
#include "pi_databuffer.h"
#include "pi_timehandler.h"

#include "glgraphcontainer.h"
#include "widgetinfo.h"

#include <qfile.h>
#include <qmutex.h>

class QWidget;
class QPushButton;

class TopDisplay;
class ViewControl;
class BaseFreqControl;
class GraphConfig;
class SpectrumData;
class FeatureData;
class FeatureControl;

class Visualize : public PI_Configuration,
				    public PI_GuiContainer,
					public PI_DataBufferConsumer,
					public PI_TimeConsumer,
					virtual public PI_Unknown
{
public:
	Visualize( );

	virtual ~Visualize();

	virtual void connectToProvider( PI_TimeProvider * provider )
	{
		m_pTimeProvider = provider;
	}

	virtual void setParentWindow( QWidget * pParent, const QString & widget );
	virtual void connectSlot( const QString & signal, const QObject * obj, const QString & slot );
	virtual QObject * getObject( const QString & obj );
	virtual QWidget * getWidget( const QString & widget );

	virtual bool configure(PluginNode * config);
	virtual bool stopRunning();
	virtual bool startRunning();

	virtual void connectToProvider( PI_DataBufferProvider * provider )
	{
		if(provider)
		{
			m_pDataBufferProvider = provider;
			m_pCompoundReader = m_pDataBufferProvider->getDataReader( PS_STR_ANALYSIS );
		}
	}

	virtual QString name();
	virtual int queryInterface( const QUuid piid, void ** ppInterface )
	{
		QMutexLocker l(&m_refLock);
		if( piid == PIID_UNKNOWN )
		{
		    *ppInterface = (PI_Unknown*)this;
		    addRef();
		}
		else if( piid == PIID_CONFIGURATION )
		{
			*ppInterface = (PI_Configuration*)this;
			addRef();
		}
		else if( piid == PIID_GUI_CONTAINER )
		{
			*ppInterface = (PI_GuiContainer*)this;
			addRef();
		}
		else if( piid == PIID_DATA_BUFFER_CONSUMER )
		{
			*ppInterface = (PI_DataBufferConsumer*)this;
			addRef();
		}
		else if( piid == PIID_TIME_CONSUMER )
		{
			*ppInterface = (PI_TimeConsumer*)this;
			addRef();
		}
		else
		{
			*ppInterface = NULL;
			return -1;
		}
		return 0;
	}

	virtual int release()
	{
		int tempcount;
		m_refLock.lock();
		if( !--m_refcount )
		{
			m_refLock.unlock();
			delete this;
			return 0;
		}
		tempcount = m_refcount;
		m_refLock.unlock();
		return tempcount;
	}

	virtual int addRef()
	{
		QMutexLocker l(&m_refLock);
		m_refcount++;
		//qDebug( "addRef: Visualize reference count: %d", m_refcount );
		return m_refcount;
	}

	DataReader * m_pCompoundReader;

	PI_TimeProvider * m_pTimeProvider;
	PI_DataBufferProvider * m_pDataBufferProvider;

private:

	WidgetInfo<GlGraphContainer> m_Graph;
	WidgetInfo<TopDisplay> m_TopDisplay;
	WidgetInfo<ViewControl> m_View1;
	WidgetInfo<ViewControl> m_View2;
	WidgetInfo<ViewControl> m_View3;
	WidgetInfo<FeatureControl> m_View4;
	WidgetInfo<FeatureControl> m_View5;
	WidgetInfo<BaseFreqControl> m_BfControl;
	WidgetInfo<GraphConfig> m_Config;
	SpectrumData * m_pSpectrumData;
	FeatureData * m_pFeatureData;

    int m_refcount;
	QMutex m_refLock;

};

#endif // SIMPLE_GRAPH_H
