TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt dll debug warn_on thread stl rtti

INCLUDEPATH	+= . ../../include ../../utility

LIBS += -lportaudio

HEADERS	 = portaudio_wavein.h
HEADERS += portaudio_waveincontrol.h
HEADERS += ../../include/prisma_defines.h
HEADERS += ../../include/plugin_global.h
HEADERS += ../../utility/dataobj.h
HEADERS += ../../utility/databuffer.h
HEADERS += ../../utility/streamcontainer.h
HEADERS += ../../utility/widgetinfo.h
HEADERS += ../../utility/safebuffer.h
HEADERS += ../../include/pi_databuffer.h
HEADERS += ../../include/pi_timehandler.h
HEADERS += ../../include/pi_configuration.h
HEADERS += ../../include/pi_guicontainer.h
HEADERS += ../../include/pi_unknown.h
HEADERS += ../../utility/config/node_macro.h
HEADERS += ../../utility/config/node.h
HEADERS += ../../utility/config/container_node.h
HEADERS += ../../utility/config/value_node.h
HEADERS += ../../utility/config/object_node.h
HEADERS += ../../utility/config/plugin_node.h
HEADERS += ../../utility/config/simplevalue_node.h
HEADERS += ../../utility/config/objectvalue_node.h

SOURCES	 = portaudio_wavein.cpp
SOURCES += portaudio_waveincontrol.cpp
SOURCES += ../../utility/databuffer.cpp
SOURCES += ../../utility/streamcontainer.cpp

#The following line was changed from FORMS to FORMS3 by qt3to4
FORMS3	= portaudio_waveincontrolbase.ui

TARGET = portaudio_wavein

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
QT += xml  qt3support 
#The following line was inserted by qt3to4
CONFIG += uic3

