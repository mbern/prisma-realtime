TEMPLATE	= lib
LANGUAGE	= C++

win32-g++:QMAKE_LFLAGS += -L"../../lib"
win32-msvc*:QMAKE_LFLAGS += /LIBPATH:"..\\..\\lib"

CONFIG	+= qt dll debug warn_on thread stl rtti opengl

INCLUDEPATH	+= . ../../include ../../utility

DEFINES += QT_CLEAN_NAMESPACE

win32:LIBS += -lglew32
unix:LIBS += -lGLEW -lGLU

HEADERS	 = visualize.h
HEADERS	+= glgraphcontainer.h
HEADERS	+= glgraphbase.h
HEADERS	+= spectrumgraph.h
HEADERS	+= featuregraph.h
HEADERS	+= glfont.h
HEADERS	+= viewcontrol.h
HEADERS	+= basefreqcontrol.h
HEADERS += graphconfig.h
HEADERS	+= topdisplay.h
HEADERS += marker.h
HEADERS += graphdefs.h
HEADERS += spectrumdata.h
HEADERS += featuredata.h
HEADERS += basegraph.h
HEADERS += featurecontrol.h
HEADERS += ../../include/prisma_defines.h
HEADERS += ../../include/plugin_global.h
HEADERS	+= ../../include/pi_configuration.h
HEADERS	+= ../../include/pi_guicontainer.h
HEADERS	+= ../../include/pi_unknown.h
HEADERS	+= ../../include/pi_databuffer.h
HEADERS	+= ../../include/pi_timehandler.h
HEADERS	+= ../../include/pi_dataobj.h
HEADERS	+= ../../utility/databuffer.h
HEADERS += ../../utility/streamcontainer.h
HEADERS += ../../utility/widgetinfo.h
HEADERS += ../../utility/doublebuffer.h
HEADERS += ../../utility/safebuffer.h
HEADERS += ../../utility/config/node_macro.h
HEADERS += ../../utility/config/node.h
HEADERS += ../../utility/config/container_node.h
HEADERS += ../../utility/config/value_node.h
HEADERS += ../../utility/config/object_node.h
HEADERS += ../../utility/config/plugin_node.h
HEADERS += ../../utility/config/simplevalue_node.h
HEADERS += ../../utility/config/objectvalue_node.h

SOURCES	 = visualize.cpp
SOURCES	+= glgraphcontainer.cpp
SOURCES	+= spectrumgraph.cpp
SOURCES	+= featuregraph.cpp
SOURCES	+= glfont.cpp
SOURCES	+= viewcontrol.cpp
SOURCES	+= basefreqcontrol.cpp
SOURCES += graphconfig.cpp
SOURCES	+= topdisplay.cpp
SOURCES += marker.cpp
SOURCES += spectrumdata.cpp
SOURCES += featuredata.cpp
SOURCES += basegraph.cpp
SOURCES += featurecontrol.cpp
SOURCES += ../../utility/doublebuffer.cpp

#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS3  = viewcontrolbase.ui
#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS3 += basefreqcontrolbase.ui
#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS3 += graphconfigbase.ui
#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS3 += topdisplaybase.ui
#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS3 += featurecontrolbase.ui

TARGET = visualize

unix {
  DESTDIR = ../../bin
  QMAKE_CFLAGS += -fvisibility=hidden
  QMAKE_CXXFLAGS += -fvisibility=hidden
  
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

win32 {
  DLLDESTDIR = ../../bin
  
  UI_DIR = tmp/ui
  MOC_DIR = tmp/moc
  OBJECTS_DIR = tmp/obj
}
#The following line was inserted by qt3to4
QT += xml  opengl qt3support 
#The following line was inserted by qt3to4
CONFIG += uic3

