TEMPLATE = lib

unix:QMAKE_LFLAGS += -L"../../lib"
win32-g++:QMAKE_LFLAGS += -L"../../lib"
win32-msvc*:QMAKE_LFLAGS += /LIBPATH:"..\\..\\lib"

INCLUDEPATH	+= . ../../include ../../utility

win32:LIBS += -llibfftw3-3 -lcblas_win32 -lblas_win32
unix:LIBS += -lfftw3 -lblas
		
CONFIG	+= qt dll debug warn_on thread stl rtti

TARGET = analyse_os

HEADERS  = analyse_os.h
HEADERS += fundfreq.h
HEADERS += ../../include/prisma_defines.h
HEADERS += ../../include/plugin_global.h
HEADERS += ../../include/pi_configuration.h
HEADERS += ../../include/pi_unknown.h
HEADERS += ../../include/pi_dataobj.h
HEADERS += ../../include/pi_guicontainer.h
HEADERS += ../../include/pi_databuffer.h
HEADERS += ../../utility/databuffer.h
HEADERS += ../../utility/streamcontainer.h
HEADERS += ../../utility/dataobj.h
HEADERS += ../../utility/config/node_macro.h
HEADERS += ../../utility/config/node.h
HEADERS += ../../utility/config/container_node.h
HEADERS += ../../utility/config/value_node.h
HEADERS += ../../utility/config/object_node.h
HEADERS += ../../utility/config/plugin_node.h
HEADERS += ../../utility/config/simplevalue_node.h
HEADERS += ../../utility/config/objectvalue_node.h

SOURCES  = analyse_os.cpp
SOURCES += fundfreq.cpp
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
