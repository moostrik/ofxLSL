meta:
    ADDON_NAME = ofxLSL
    ADDON_DESCRIPTION = Interface for Lab Streaming Layer
    ADDON_AUTHOR = Jean Jacques Warmerdam & Matthias Oostrik
    ADDON_TAGS =
    ADDON_URL = http://github.com/moostrik/ofxLSL

common:
    ADDON_INCLUDES = libs/labstreaminglayer/include
    ADDON_INCLUDES += src
    ADDON_DEPENDENCIES = ofxXmlSettings
    ADDON_CPPFLAGS = -D_x64

linux64:
    ADDON_LIBS = libs/labstreaminglayer/lib/linux64/liblsl.so
    ADDON_CPPFLAGS += -D_LINUX

osx:
