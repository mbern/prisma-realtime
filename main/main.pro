TEMPLATE	= app
LANGUAGE	= C++

CONFIG += qt debug warn_on thread stl rtti opengl

INCLUDEPATH += . ../include ../utility

HEADERS  = messageoutput.h
HEADERS += configuration.h
HEADERS += configselector.h
HEADERS += ../include/pi_unknown.h
HEADERS += ../include/pi_guicontainer.h
HEADERS += ../include/pi_configuration.h
HEADERS += ../include/pi_databuffer.h
HEADERS += ../utility/config/node_macro.h
HEADERS += ../utility/config/node.h
HEADERS += ../utility/config/container_node.h
HEADERS += ../utility/config/value_node.h
HEADERS += ../utility/config/config_node.h
HEADERS += ../utility/config/global_node.h
HEADERS += ../utility/config/application_node.h
HEADERS += ../utility/config/object_node.h
HEADERS += ../utility/config/plugin_node.h
HEADERS += ../utility/config/plugins_node.h
HEADERS += ../utility/config/document_node.h
HEADERS += ../utility/config/simplevalue_node.h
HEADERS += ../utility/config/objectvalue_node.h
HEADERS += ../utility/config/proxyvalue_node.h

SOURCES	 = main.cpp
SOURCES	+= messageoutput.cpp
SOURCES += configuration.cpp
SOURCES += configselector.cpp

#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS3 += config_selector.ui

TARGET	= prisma

RC_FILE = prisma.rc

DESTDIR	= ../bin

unix:QMAKE_LFLAGS += -Wl,--rpath=.

unix:LIBS += -lX11

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

win32 {
  UI_DIR = tmp/ui
  MOC_DIR = tmp/moc
  OBJECTS_DIR = tmp/obj
}
#The following line was inserted by qt3to4
QT += xml  opengl qt3support 
#The following line was inserted by qt3to4
CONFIG += uic3

