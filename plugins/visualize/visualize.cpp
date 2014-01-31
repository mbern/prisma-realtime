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

#include "visualize.h"

#include <qwidget.h>
#include <qpushbutton.h>
#include <qdial.h>
#include <qlayout.h>
#include <q3vbox.h>
#include <q3hbox.h>
#include <qsizepolicy.h>
#include <qradiobutton.h>
#include <q3buttongroup.h>
#include <qtoolbox.h>
#include <qspinbox.h>

#include "viewcontrol.h"
#include "basefreqcontrol.h"
#include "graphconfig.h"
#include "topdisplay.h"
#include "databuffer.h"
#include "spectrumgraph.h"
#include "featuregraph.h"
#include "spectrumdata.h"
#include "featuredata.h"
#include "featurecontrol.h"

#include "config/node_macro.h"

#ifdef Q_OS_WIN
#include <GL/wglew.h>
#endif //Q_OS_WIN

#ifdef Q_WS_X11
#include <GL/glxew.h>
#endif //Q_WS_X11

Visualize::Visualize( )
: m_pCompoundReader(NULL),m_pTimeProvider(NULL), m_pDataBufferProvider(NULL),
  m_Graph("Graph"),m_TopDisplay("TopDisplay"),m_View1("Diagram1"),
  m_View2("Diagram2"),m_View3("Diagram3"),m_View4("Diagram4"),m_View5("Diagram5"),
  m_BfControl("BaseFrequency"),m_Config("Configuration"),
  m_pSpectrumData(NULL), m_pFeatureData(NULL), m_refcount(0), m_refLock(TRUE)
{
}

Visualize::~Visualize()
{
	if(m_pSpectrumData)
		delete m_pSpectrumData;
	if(m_pFeatureData)
		delete m_pFeatureData;
	if(m_pCompoundReader)
		delete m_pCompoundReader;
	if(m_pTimeProvider)
		m_pTimeProvider->release();
	if(m_pDataBufferProvider)
		m_pDataBufferProvider->release();
}

void Visualize::connectSlot( const QString & signal, const QObject * obj, const QString & slot )
{
	QString slot_str = SLOT(mySlot);
	QString slot_fcn = slot;

	if( signal == "pause" )
	{
		slot_fcn.append( "(bool)" );
		slot_str.replace( "mySlot", slot_fcn );
		m_TopDisplay.widget->connect( m_TopDisplay.widget, SIGNAL(pause(bool)), obj, slot_str );
	}
}

void Visualize::setParentWindow( QWidget * pParent, const QString & widget )
{
	if( widget == m_View1.name )
		m_View1.parent = pParent;
	else if( widget == m_View2.name )
		m_View2.parent = pParent;
	else if( widget == m_View3.name )
		m_View3.parent = pParent;
	else if( widget == m_View4.name )
		m_View4.parent = pParent;
	else if( widget == m_View5.name )
		m_View5.parent = pParent;
	else if( widget == m_Config.name )
		m_Config.parent = pParent;
	else if( widget == m_BfControl.name )
		m_BfControl.parent = pParent;
	else if( widget == m_TopDisplay.name)
		m_TopDisplay.parent = pParent;
	else if( widget == m_Graph.name )
		m_Graph.parent = pParent;
}

QObject * Visualize::getObject( const QString & obj )
{
	if( obj == m_View1.name )
		return m_View1.widget;
	else if( obj == m_View2.name )
		return m_View2.widget;
	else if( obj == m_View3.name )
		return m_View3.widget;
	else if( obj == m_View4.name )
		return m_View4.widget;
	else if( obj == m_View5.name )
		return m_View5.widget;
	else if( obj == m_Config.name )
		return m_Config.widget;
	else if( obj == m_BfControl.name )
		return m_BfControl.widget;
	else if( obj == m_TopDisplay.name)
		return m_TopDisplay.widget;
	else if( obj == m_Graph.name )
		return m_Graph.widget;
	else
        return NULL;
}

QWidget * Visualize::getWidget( const QString & widget )
{
	if( widget == m_View1.name )
		return m_View1.widget;
	else if( widget == m_View2.name )
		return m_View2.widget;
	else if( widget == m_View3.name )
		return m_View3.widget;
	else if( widget == m_View4.name )
		return m_View4.widget;
	else if( widget == m_View5.name )
		return m_View5.widget;
	else if( widget == m_Config.name )
		return m_Config.widget;
	else if( widget == m_BfControl.name )
		return m_BfControl.widget;
	else if( widget == m_TopDisplay.name)
		return m_TopDisplay.widget;
	else if( widget == m_Graph.name )
		return m_Graph.widget;
	else
        return NULL;
}


bool Visualize::configure(PluginNode * config)
{
	QGLFormat glformat( QGL::DoubleBuffer );

	OBJECTNODE_DECLARE_AND_GET(m_Graph,GlGraphContainer,config);
	m_Graph.widget = new GlGraphContainer( NODE_VARIABLE_NAME(m_Graph), this, glformat, m_Graph.parent );

	m_Graph.widget->makeCurrent();

	// Initialize GLEW
	GLenum err = glewInit();
	if(err != GLEW_OK)
		qFatal("Unable to initialize GLEW");

	if(GLEW_ARB_vertex_buffer_object)
		qDebug("GL_ARB_vertex_buffer_object supported.");
	else
		qFatal("GL_ARB_vertex_buffer_object NOT supported.");

#ifdef Q_OS_WIN
	if(WGLEW_EXT_swap_control)
		qDebug("WGL_EXT_swap_control supported.");
	else
		qFatal("WGL_EXT_swap_control NOT supported");
#endif //Q_OS_WIN

#ifdef Q_WS_X11
	if(GLXEW_SGI_swap_control)
		qDebug("GLX_SGI_swap_control supported.");
	else
		qFatal("GLX_SGI_swap_control NOT supported.");
#endif //Q_WS_X11

	m_Graph.widget->doneCurrent();

	OBJECTNODE_DECLARE_AND_GET(m_SpectrumData, SpectrumData, config);
	m_pSpectrumData = new SpectrumData( NODE_VARIABLE_NAME(m_SpectrumData) );

	OBJECTNODE_DECLARE_AND_GET(m_FeatureData, SpectrumData, config);
	m_pFeatureData = new FeatureData( NODE_VARIABLE_NAME(m_FeatureData) );

	m_Graph.widget->addDataObject( m_pSpectrumData );
	m_Graph.widget->addDataObject( m_pFeatureData );

	BaseGraph * pSGraph1 = new SpectrumGraph( m_Graph.widget, 1, m_pSpectrumData );
	BaseGraph * pSGraph2 = new SpectrumGraph( m_Graph.widget, 2, m_pSpectrumData );
	BaseGraph * pSGraph3 = new SpectrumGraph( m_Graph.widget, 3, m_pSpectrumData );
	BaseGraph * pFGraph1 = new FeatureGraph( m_Graph.widget, 4, m_pFeatureData );
	BaseGraph * pFGraph2 = new FeatureGraph( m_Graph.widget, 5, m_pFeatureData );

	Marker * pSG1_Fl = new Marker( pSGraph3, 5, 0xAAAA, 1);
	Marker * pSG1_Fh = new Marker( pSGraph3, 5, 0xAAAA, 1);
	Marker * pSG2_Fl = new Marker( pSGraph3, 5, 0xD6D6, 1);
	Marker * pSG2_Fh = new Marker( pSGraph3, 5, 0xD6D6, 1);
	Marker * pSG1_ref = new Marker( pSGraph1, 1, 0xFFFF, 1);
	Marker * pSG2_ref = new Marker( pSGraph2, 1, 0xFFFF, 1);
	Marker * pSG3_ref = new Marker( pSGraph3, 1, 0xFFFF, 1);
	Marker * pSG4_ref = new Marker( pFGraph1, 1, 0xFFFF, 1);
	Marker * pSG5_ref = new Marker( pFGraph2, 1, 0xFFFF, 1);

	m_Graph.widget->addGraphObject( pFGraph2 );
	m_Graph.widget->addGraphObject( pFGraph1 );
	m_Graph.widget->addGraphObject( pSGraph3 );
	m_Graph.widget->addGraphObject( pSGraph2 );
	m_Graph.widget->addGraphObject( pSGraph1 );


	// Diagram 1
	OBJECTNODE_DECLARE_AND_GET(m_View1,ViewControl,config);
	m_View1.widget = new ViewControl( NODE_VARIABLE_NAME(m_View1), TF_Atlas | TA_Atlas | TF_Spectrum, false, m_View1.parent ); //pToolBox );
	pSGraph1->connect( m_View1.widget, SIGNAL( newState( const ViewState& ) ), SLOT( setValues( const ViewState& ) ) );
	pSG1_Fl->connect( m_View1.widget, SIGNAL( newFLow( double ) ), SLOT( setValue( double ) ) );
	pSG1_Fh->connect( m_View1.widget, SIGNAL( newFHigh( double ) ), SLOT( setValue( double ) ) );
	pSG1_ref->connect( m_View1.widget, SIGNAL( showToneMarker( bool  ) ), SLOT( setVisible( bool ) ) );
	pSG1_ref->connect( m_View1.widget, SIGNAL( setToneMarkerDir( int  ) ), SLOT( setDirection( int ) ) );


	// Diagram 2
	OBJECTNODE_DECLARE_AND_GET(m_View2,ViewControl,config);
	m_View2.widget = new ViewControl( NODE_VARIABLE_NAME(m_View2), TF_Atlas | TA_Atlas, true, m_View2.parent ); //pToolBox );
	pSGraph2->connect( m_View2.widget, SIGNAL( newState( const ViewState& ) ), SLOT( setValues( const ViewState& ) ) );
	pSG2_Fl->connect( m_View2.widget, SIGNAL( newFLow( double ) ), SLOT( setValue( double ) ) );
	pSG2_Fh->connect( m_View2.widget, SIGNAL( newFHigh( double ) ), SLOT( setValue( double ) ) );
	pSG2_ref->connect( m_View2.widget, SIGNAL( showToneMarker( bool  ) ), SLOT( setVisible( bool ) ) );
	pSG2_ref->connect( m_View2.widget, SIGNAL( setToneMarkerDir( int  ) ), SLOT( setDirection( int ) ) );

	// Diagram 3
	OBJECTNODE_DECLARE_AND_GET(m_View3,ViewControl,config);
	m_View3.widget = new ViewControl( NODE_VARIABLE_NAME(m_View3), FA_Atlas, false, m_View3.parent ); //pToolBox );
	pSGraph3->connect( m_View3.widget, SIGNAL( newState( const ViewState& ) ), SLOT( setValues( const ViewState& ) ) );
	pSG3_ref->connect( m_View3.widget, SIGNAL( showToneMarker( bool  ) ), SLOT( setVisible( bool ) ) );
	pSG3_ref->connect( m_View3.widget, SIGNAL( setToneMarkerDir( int  ) ), SLOT( setDirection( int ) ) );

	// Diagram 4
	OBJECTNODE_DECLARE_AND_GET(m_View4,ViewControl,config);
	m_View4.widget = new FeatureControl( NODE_VARIABLE_NAME(m_View4), m_View4.parent );
	pFGraph1->connect( m_View4.widget, SIGNAL( newState( const ShorttimeFeatureState& ) ), SLOT( setValues( const ShorttimeFeatureState& ) ) );
	pSG4_ref->connect( m_View4.widget, SIGNAL( showToneMarker( bool ) ), SLOT( setVisible( bool ) ) );
	pSG4_ref->setDirection( Marker::M_H );

	// Diagram 5
	OBJECTNODE_DECLARE_AND_GET(m_View5,ViewControl,config);
	m_View5.widget = new FeatureControl( NODE_VARIABLE_NAME(m_View5), m_View5.parent );
	pFGraph2->connect( m_View5.widget, SIGNAL( newState( const ShorttimeFeatureState& ) ), SLOT( setValues( const ShorttimeFeatureState& ) ) );
	pSG5_ref->connect( m_View5.widget, SIGNAL( showToneMarker( bool ) ), SLOT( setVisible( bool ) ) );
	pSG5_ref->setDirection( Marker::M_H );

	// Bypass
	m_View2.widget->connect( m_View1.widget, SIGNAL( newState( const ViewState& ) ), SLOT( newBypassData( const ViewState& ) ) );
	m_View1.widget->connect( m_View2.widget, SIGNAL( trigger( ) ), SLOT( sendState( ) ) );

	// Konfiguration
	OBJECTNODE_DECLARE_AND_GET(m_Config,GraphConfig,config);
	m_Config.widget = new GraphConfig( NODE_VARIABLE_NAME(m_Config), m_Config.parent ); //pToolBox );
	m_pSpectrumData->connect( m_Config.widget, SIGNAL(deltaCChanged(int)), SLOT(setDeltaC(int)) );
	m_Graph.widget->connect( m_Config.widget, SIGNAL(backgroundGrayChanged(unsigned char)), SLOT(setBackgroundGray(unsigned char)) );
	m_Graph.widget->connect( m_Config.widget, SIGNAL(pointSizeChanged(int)), SLOT(setPointSize(int)) );
	pSGraph1->connect( m_Config.widget, SIGNAL(backgroundGrayChanged(unsigned char)), SLOT(setGrayLevel(unsigned char)) );
	pSGraph2->connect( m_Config.widget, SIGNAL(backgroundGrayChanged(unsigned char)), SLOT(setGrayLevel(unsigned char)) );
	pSGraph3->connect( m_Config.widget, SIGNAL(backgroundGrayChanged(unsigned char)), SLOT(setGrayLevel(unsigned char)) );
	pFGraph1->connect( m_Config.widget, SIGNAL(backgroundGrayChanged(unsigned char)), SLOT(setGrayLevel(unsigned char)) );
	pFGraph2->connect( m_Config.widget, SIGNAL(backgroundGrayChanged(unsigned char)), SLOT(setGrayLevel(unsigned char)) );

	// Grundfrequenz
	OBJECTNODE_DECLARE_AND_GET(m_BfControl,BaseFreqControl,config);
	m_BfControl.widget = new BaseFreqControl( NODE_VARIABLE_NAME(m_BfControl), m_BfControl.parent ); //pToolBox );
	m_BfControl.widget->connect( m_Graph.widget, SIGNAL( newBaseFreq(double) ), SLOT( setBaseFreq(double) ) );
	m_View1.widget->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setRefFreq(double) ) );
	m_View2.widget->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setRefFreq(double) ) );
	m_View3.widget->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setRefFreq(double) ) );
	m_View4.widget->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setRefFreq(double) ) );
	m_View5.widget->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setRefFreq(double) ) );
	pSG1_ref->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setValue(double) ) );
	pSG2_ref->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setValue(double) ) );
	pSG3_ref->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setValue(double) ) );
	pSG4_ref->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setValue(double) ) );
	pSG5_ref->connect( m_BfControl.widget, SIGNAL( newRefFreq(double) ), SLOT( setValue(double) ) );
	m_pFeatureData->connect( m_BfControl.widget, SIGNAL( newTuning(int) ), SLOT( newTuning(int) ) );

	// Top Display (Grundfrequenz und Pause)
	OBJECTNODE_DECLARE_AND_GET(m_TopDisplay,TopDisplay,config);
	m_TopDisplay.widget = new TopDisplay( NODE_VARIABLE_NAME(m_TopDisplay), m_TopDisplay.parent ); //pVBox );
	m_TopDisplay.widget->connect( m_BfControl.widget, SIGNAL( newBaseFreqDisplay(QStringList) ), SLOT( setBaseFreqDisplay(QStringList) ) );
	m_TopDisplay.widget->connect( m_Graph.widget, SIGNAL( newMeterValue(double,double) ), SLOT( setMeterValue(double,double) ) );
	m_Graph.widget->connect( m_TopDisplay.widget, SIGNAL( pause(bool) ), SLOT( setPause(bool) ) );

	// Initialisierung
	m_View1.widget->sendState();
	m_View2.widget->sendState();
	m_View3.widget->sendState();
	m_View4.widget->sendState();
	m_View5.widget->sendState();
	m_Config.widget->sendState();
	m_BfControl.widget->sendState();
	m_TopDisplay.widget->sendState();

	return TRUE;
}

bool Visualize::stopRunning()
{
//	m_pGraph->stop();
	return true;
}

bool Visualize::startRunning()
{
	m_Graph.widget->start(  );
	return true;
}

QString Visualize::name()
{
	return QString( "simplegraph" );
}


extern "C" {
PLUGIN_EXPORT void LoadObject( PI_Unknown ** ppUnknown )
{
	Visualize *obj = new Visualize();
	obj->queryInterface( PIID_UNKNOWN, (void**)ppUnknown );
}
} //extern "C"
