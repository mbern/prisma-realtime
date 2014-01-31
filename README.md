Prisma-Realtime is a software which analyzes and visualizes monophonic musical
instrument tones in real-time

The software runs on Windows. With not too much effort it should be possible
to make it running on Linux or OS X.

Required libraries
-------------------
- QT4  (http://qt-project.org/downloads)
- GLEW (http://glew.sourceforge.net)
- FFTW (http://www.fftw.org/install/windows.html)
- BLAS (http://www.netlib.org/blas)

Building the software on Windows
--------------------------------

On Windows, put the header files for GLEW, FFTW and BLAS into the folder
'include':

include/GL/glew.h
include/GL/wglew.h
include/cblas.h
include/fftw3.h

Put the lib-files into the folder 'lib':

lib/blas_win32.lib
lib/cblas_win32.lib
lib/glew32.lib
lib/glew32s.lib
lib/libfftw3-3.lib

Make sure the compiler (e.g. Visual Studio 2010) is set-up in your command
shell.

Set the QTDIR and set the path to qmake.exe

Call 'qmake -recursive -tp vc' to create Visual Studio project files.
Alternatively you can use 'qmake -recursive' followed by 'nmake'

Prisma-Realtime will be built in the folder 'bin'. In order to run all the
shared libraries (QT, GLEW and FFTW) must be present in this folder.

