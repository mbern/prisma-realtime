TEMPLATE = subdirs

# The application
SUBDIRS  = main

# Plugins

# Windows specific
win32:SUBDIRS += plugins/wavein

# New wavein driver. Uses Portaudio and should be portable.
# only testet under Linux howerver
unix:SUBDIRS += plugins/portaudio_wavein

# Common
SUBDIRS += plugins/visualize
SUBDIRS += plugins/filewriter
SUBDIRS += plugins/analyse_os

# Testing
SUBDIRS += plugins/dummy_wavein
SUBDIRS += plugins/dummy_analyse
