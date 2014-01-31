TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt dll debug warn_on thread stl rtti

INCLUDEPATH	+= . ../../include ../../utility

HEADERS	 = dummy_wavein.h
HEADERS += ../../include/prisma_defines.h
HEADERS += ../../include/plugin_global.h
HEADERS += ../../include/pi_databuffer.h
HEADERS += ../../include/pi_timehandler.h
HEADERS += ../../include/pi_unknown.h
HEADERS += ../../include/pi_configuration.h
HEADERS += ../../utility/dataobj.h
HEADERS += ../../utility/databuffer.h
HEADERS += ../../utility/streamcontainer.h

SOURCES	 = dummy_wavein.cpp
SOURCES += ../../utility/databuffer.cpp
SOURCES += ../../utility/streamcontainer.cpp

TARGET = dummy_wavein

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
