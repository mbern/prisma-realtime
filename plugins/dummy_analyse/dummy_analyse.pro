TEMPLATE = lib

INCLUDEPATH	+= . ../../include ../../utility

CONFIG	+= qt dll debug warn_on thread stl rtti

TARGET = dummy_analyse

HEADERS  = dummy_analyse.h
HEADERS += ../../include/plugin_global.h
HEADERS += ../../include/pi_configuration.h
HEADERS += ../../include/pi_unknown.h
HEADERS += ../../include/pi_dataobj.h
HEADERS += ../../include/pi_databuffer.h
HEADERS += ../../utility/databuffer.h
HEADERS += ../../utility/streamcontainer.h
HEADERS += ../../utility/dataobj.h

SOURCES  = dummy_analyse.cpp
SOURCES += ../../utility/databuffer.cpp
SOURCES += ../../utility/streamcontainer.cpp

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
