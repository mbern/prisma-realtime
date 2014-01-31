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

#include <qapplication.h>

#include <q3accel.h>
#include <qfile.h>
#include <qxml.h>
#include <q3textstream.h>
#include <qobject.h>
#include <qlibrary.h>
#include <q3grid.h>
#include <q3mainwindow.h>
#include <qdialog.h>
#include <q3vbox.h>
#include <qtoolbox.h>
#include <qlayout.h>
#include <qsettings.h>
#include <qfileinfo.h>

#include "pi_unknown.h"
#include "pi_guicontainer.h"
#include "pi_configuration.h"
#include "pi_databuffer.h"
#include "pi_timehandler.h"

#include "messageoutput.h"
#include "configuration.h"

#include "configselector.h"

#include "config/node_macro.h"
#include "config/application_node.h"
#include "config/plugins_node.h"
#include "config/config_node.h"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

void dumpall( const QObjectList& olist )
{
	for (QObjectList::ConstIterator it = olist.begin(); it != olist.end(); ++it)
	{
		qDebug( "---------------------------------------" );
		qDebug( "Pointer: %p", *it );
		(*it)->dumpObjectInfo();

		dumpall( (*it)->children() );
	}
}


class Plugin : public QLibrary
{
public:
	Plugin(const char * name)
	: QLibrary(name),obj(NULL),plugin_name(name)
	{
		LOADPLUGIN pfnLoad = NULL;
		pfnLoad = (LOADPLUGIN)this->resolve("LoadObject");
		if(pfnLoad != NULL)
		{
			pfnLoad(&obj);
			qDebug("Plugin %s loaded!",name);
		}
	}
	~Plugin()
	{
		if(obj != NULL)
			obj->release();
	}
	PI_Unknown* getObject()
	{
		if(obj)
		{
			obj->addRef();
			return obj;
		}
		else
			return NULL;
	}
private:
	typedef void (*LOADPLUGIN)( PI_Unknown ** );
	PI_Unknown *obj;
	QString plugin_name;
};


int main( int argc, char ** argv )
{
	// Register custom message handler if desired
	bool handler_registered = FALSE;
	MyMessageOutput * mymsghandler = NULL;
	for(int i = 1; i < argc; i++)
	{
		QString arg(argv[i]);
		QString opt("-msg_handler");
		if(arg.startsWith(opt))
		{
			// Extract handler type
			QString handler = arg.section("=",1,1);
			if(handler.isEmpty())
			{
				qWarning("Option '-msg_handler' is not complete. Use default handler.");
				handler_registered = TRUE;
				break;
			}
			if(handler == "console")
			{
				handler_registered = TRUE;
				break;
			}
			if(!handler.endsWith(".txt"))
			{
				qWarning("File '%s' does not end with '.txt'. Use default handler.",handler.ascii());
				handler_registered = TRUE;
				break;
			}
			QFileInfo hfinfo(handler);
			if(hfinfo.exists())
			{
				if(!hfinfo.isWritable())
				{
					qWarning("Cannot write to '%s'. Use default handler",handler.ascii());
					handler_registered = TRUE;
					break;
				}
			}
			else
			{
				QString path = hfinfo.dirPath();
				QFileInfo hfdinfo(path);
				if(!hfdinfo.isWritable())
				{
					qWarning("Cannot create file in dir '%s'. Use default handler",path.ascii());
					handler_registered = TRUE;
					break;
				}

			}
			mymsghandler = new MyMessageOutput(handler);
			handler_registered = TRUE;
			break;
		}
	}
	if(!handler_registered)
	{
		mymsghandler = new MyMessageOutput(QString("debuglog.txt"));
		handler_registered = TRUE;
	}

#ifdef Q_WS_X11
	if(XInitThreads() == 0)
		qFatal("XInitThreads didn't succeed");
#endif

	// Create a QApplication object and add default close
    QApplication a( argc, argv );
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );

	// Start the configuration selection dialog if more than one config file is available
	ConfigSelector cs;
	if(cs.getNumConfigs() > 1)
	{
		cs.resize(300,150);
		cs.show();
		a.exec();
	}

	// Load configuration
	Configuration * config = new Configuration();
	QString configfile(cs.getConfig());
	if(!config->load(configfile))
	{
		qFatal("Unable to load %s", configfile.ascii());
	}

	ConfigNode * cn = config->getConfig();
	if(!cn)
	{
		qFatal("Unable to get ConfigNode");
	}

	ApplicationNode * an = cn->getApplicationNode();
	if(!an)
	{
		qFatal("Unable to get ApplicationNode");
	}

	PluginsNode * pn = cn->getPluginsNode();
	if(!pn)
	{
		qFatal("Unable to get PluginsNode");
	}

	OBJECTVALUENODE_DECLARE_AND_GET(QPoint,position,an);
	OBJECTVALUENODE_DECLARE_AND_GET(QSize,size,an);
	SIMPLEVALUENODE_DECLARE_AND_GET(bool,maximized,an);
	SIMPLEVALUENODE_DECLARE_AND_GET(Q_INT8,tbindex,an);

    // Create the toplevel window
	Q3HBox * w = new Q3HBox();
	w->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

	// Add a widget that contains the toolbox and measurements
	Q3VBox * pVBox = new Q3VBox( w );
	pVBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
	pVBox->setSpacing( 5 );
	pVBox->setMargin( 2 );

	// Create the toolbox
	QToolBox * tb = new QToolBox( pVBox );

	// Create all plugins
    Plugin * VisualizePlugin = new Plugin("visualize");
    Plugin * AnalysePlugin = new Plugin("analyse_os");
    Plugin * WaveInPlugin = new Plugin("wavein");
    Plugin * FileWriterPlugin = new Plugin("filewriter");
    // Load plugin objects
	PI_Unknown *ukvi = VisualizePlugin->getObject();
	PI_Unknown *ukan = AnalysePlugin->getObject();
	PI_Unknown *ukwi = WaveInPlugin->getObject();
	PI_Unknown *ukfw = FileWriterPlugin->getObject();

	// Configure the system
	if( ukvi && ukan && ukwi && ukfw )
	{
		PI_GuiContainer* gcwiobj = NULL;
		PI_GuiContainer* gcviobj = NULL;
		PI_GuiContainer* gcfwobj = NULL;
		PI_Configuration* xcanobj = NULL;
		PI_Configuration* xcviobj = NULL;
		PI_Configuration* xcwiobj = NULL;
		PI_Configuration* xcfwobj = NULL;
		PI_DataBufferProvider* dbpwiobj = NULL;
		PI_DataBufferProvider* dbpanobj = NULL;
		PI_DataBufferConsumer* dbcanobj = NULL;
		PI_DataBufferConsumer* dbcviobj = NULL;
		PI_DataBufferConsumer* dbcfwobj = NULL;
		PI_TimeProvider* tpwiobj = NULL;
		PI_TimeConsumer* tcviobj = NULL;
		PI_TimeConsumer* tcfwobj = NULL;

		ukwi->queryInterface( PIID_GUI_CONTAINER, (void**)&gcwiobj );
		ukvi->queryInterface( PIID_GUI_CONTAINER , (void**)&gcviobj );
		ukfw->queryInterface( PIID_GUI_CONTAINER, (void**)&gcfwobj );
		ukan->queryInterface( PIID_CONFIGURATION, (void**)&xcanobj );
		ukvi->queryInterface( PIID_CONFIGURATION, (void**)&xcviobj );
		ukwi->queryInterface( PIID_CONFIGURATION, (void**)&xcwiobj );
		ukfw->queryInterface( PIID_CONFIGURATION, (void**)&xcfwobj );
		ukwi->queryInterface( PIID_DATA_BUFFER_PROVIDER, (void**)&dbpwiobj );
		ukan->queryInterface( PIID_DATA_BUFFER_PROVIDER, (void**)&dbpanobj );
		ukan->queryInterface( PIID_DATA_BUFFER_CONSUMER, (void**)&dbcanobj );
		ukvi->queryInterface( PIID_DATA_BUFFER_CONSUMER, (void**)&dbcviobj );
		ukfw->queryInterface( PIID_DATA_BUFFER_CONSUMER, (void**)&dbcfwobj );
		ukwi->queryInterface( PIID_TIME_PROVIDER, (void**)&tpwiobj );
		ukvi->queryInterface( PIID_TIME_CONSUMER, (void**)&tcviobj );
		ukfw->queryInterface( PIID_TIME_CONSUMER, (void**)&tcfwobj );

		if( gcwiobj && gcviobj && gcfwobj &&
			xcwiobj && xcanobj && xcviobj && xcfwobj &&
			dbpwiobj && dbpanobj && dbcanobj && dbcviobj && dbcfwobj &&
			tpwiobj && tcviobj && tcfwobj)
		{
			// Wave in object
			gcwiobj->setParentWindow( tb, "WaveDeviceConfiguration" );
			// Visualize object
    		gcviobj->setParentWindow( tb, "Diagram1" );
    		gcviobj->setParentWindow( tb, "Diagram2" );
    		gcviobj->setParentWindow( tb, "Diagram3" );
			gcviobj->setParentWindow( tb, "Diagram4" );
			gcviobj->setParentWindow( tb, "Diagram5" );
			gcviobj->setParentWindow( tb, "Configuration" );
			gcviobj->setParentWindow( tb, "BaseFrequency" );
			gcviobj->setParentWindow( pVBox, "TopDisplay" );
			gcviobj->setParentWindow( w, "Graph" );
			// File writer object
			gcfwobj->setParentWindow( tb, "SpectralDataSaveGui" );

			// Wave in object
			PLUGINNODE_DECLARE_AND_GET(wavein,pn);
			xcwiobj->configure(NODE_VARIABLE_NAME(wavein));
			// Analyse object
			PLUGINNODE_DECLARE_AND_GET(analyse,pn);
			xcanobj->configure(NODE_VARIABLE_NAME(analyse));
			// Visualize object
			PLUGINNODE_DECLARE_AND_GET(visualize,pn);
			xcviobj->configure(NODE_VARIABLE_NAME(visualize));
			// File writer object
			PLUGINNODE_DECLARE_AND_GET(filewriter,pn);
			xcfwobj->configure(NODE_VARIABLE_NAME(filewriter));

			// Connect the 'pause' source to different sinks
			gcviobj->connectSlot( "pause", gcwiobj->getObject("WaveDeviceConfiguration"), "setPause" );
			gcviobj->connectSlot( "pause", gcfwobj->getObject("SpectralDataSaveGui"), "setPause" );

			// Add visualize object configurations to the toolbox
			tb->addItem( gcviobj->getWidget("Diagram1"), "Diagramm 1" );
			tb->addItem( gcviobj->getWidget("Diagram2"), "Diagramm 2" );
			tb->addItem( gcviobj->getWidget("Diagram3"), "Diagramm 3" );
			tb->addItem( gcviobj->getWidget("Diagram4"), "Diagramm 4" );
			tb->addItem( gcviobj->getWidget("Diagram5"), "Diagramm 5" );
			tb->addItem( gcviobj->getWidget("Configuration"), "Konfiguration" );
			tb->addItem( gcviobj->getWidget("BaseFrequency"), "Bezugston, Stimmung" );
			// Add file writer object configuration to the toolbox
			tb->addItem( gcfwobj->getWidget("SpectralDataSaveGui"), "Datenspeicherung" );
			// Add wave in object configuration to the toolbox
			tb->addItem( gcwiobj->getWidget("WaveDeviceConfiguration"), "Soundkarte" );

			dbpwiobj->addRef();
			dbcanobj->connectToProvider( dbpwiobj );
			dbpanobj->addRef();
			dbcviobj->connectToProvider( dbpanobj );

			dbpwiobj->addRef();
			dbcfwobj->connectToProvider( dbpwiobj ); //filewrite
			dbpanobj->addRef();
			dbcfwobj->connectToProvider( dbpanobj ); //filewrite

			tpwiobj->addRef();
			tcviobj->connectToProvider( tpwiobj );
			tpwiobj->addRef();
			tcfwobj->connectToProvider( tpwiobj ); //filewrite

			xcwiobj->startRunning();
			xcanobj->startRunning();
			xcviobj->startRunning();
			xcfwobj->startRunning(); //filewrite

		}

		if( gcwiobj ) gcwiobj->release();
		if( gcviobj ) gcviobj->release();
		if( gcfwobj ) gcfwobj->release();
		if( xcanobj ) xcanobj->release();
		if( xcviobj ) xcviobj->release();
		if( xcwiobj ) xcwiobj->release();
		if( xcfwobj ) xcfwobj->release();
		if( dbpwiobj ) dbpwiobj->release();
		if( dbpanobj ) dbpanobj->release();
		if( dbcanobj ) dbcanobj->release();
		if( dbcviobj ) dbcviobj->release();
		if( dbcfwobj ) dbcfwobj->release();
		if( tpwiobj ) tpwiobj->release();
		if( tcviobj ) tcviobj->release();
		if( tcfwobj ) tcfwobj->release();
	}

	// Set active item in the ToolBox
	tb->setCurrentIndex(VALUENODE_GET_VALUE(tbindex));

	w->resize(VALUENODE_GET_VALUE(size));
	w->move(VALUENODE_GET_VALUE(position));
	if(VALUENODE_GET_VALUE(maximized))
		w->showMaximized();
	w->setCaption("Prisma-Realtime v1.3b");
    w->show();

    Q3Accel *esc = new Q3Accel( w );
    esc->connectItem( esc->insertItem( Qt::Key_Escape ), &a, SLOT( quit() ) );

    a.exec();

	w->hide();

	if(!w->isMaximized())
	{
		VALUENODE_SET_VALUE(size,w->size());
		VALUENODE_SET_VALUE(position,w->pos());
	}
	VALUENODE_SET_VALUE(maximized,w->isMaximized());

	VALUENODE_SET_VALUE(tbindex,tb->currentIndex());

	if( ukvi && ukan && ukwi && ukfw )
	{
		PI_Configuration * xmlconfig = NULL;

		ukvi->queryInterface(PIID_CONFIGURATION, (void**)&xmlconfig);
		if(xmlconfig != NULL)
		{
			xmlconfig->stopRunning();
			xmlconfig->release();
			xmlconfig = NULL;
		}
		ukan->queryInterface(PIID_CONFIGURATION, (void**)&xmlconfig);
		if(xmlconfig != NULL)
		{
			xmlconfig->stopRunning();
			xmlconfig->release();
			xmlconfig = NULL;
		}
		ukwi->queryInterface(PIID_CONFIGURATION, (void**)&xmlconfig);
		if(xmlconfig != NULL)
		{
			xmlconfig->stopRunning();
			xmlconfig->release();
			xmlconfig = NULL;
		}
		ukfw->queryInterface(PIID_CONFIGURATION, (void**)&xmlconfig);
		if(xmlconfig != NULL)
		{
			xmlconfig->stopRunning();
			xmlconfig->release();
			xmlconfig = NULL;
		}
	}

	if(ukvi) ukvi->release();
	if(ukan) ukan->release();
	if(ukwi) ukwi->release();
	if(ukfw) ukfw->release();

	delete w;

	config->save(configfile);
	// Delete before unloading plugins!!!
	delete config;

	// Release plugins
	delete VisualizePlugin;
	delete FileWriterPlugin;
	delete AnalysePlugin;
	delete WaveInPlugin;


	if(mymsghandler)
		delete mymsghandler;

    return 0;

}
