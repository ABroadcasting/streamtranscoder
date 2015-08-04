PACKAGE=streamTranscoder
VERSION=0.2.0
AUTOMAKE=automake

SUBDIRS=. 
EXTRA_DIST=win32 \
	dsp_oddcast_wa3 \
	wmaenc.h \
	wmaenc.cpp \
	streamTranscoderUI.def \
	streamTranscoderUI.dsp \
	streamTranscoderUI.dsw \
	streamTranscoderUI.h \
	streamTranscoderUI.rc \
	transcoder.ico \
	transcoder.xpm

liboddcast_config_a_SOURCES=\
	liboddcast_wxconfig.h \
	liboddcast_wxconfig.cpp

LIBODDCAST_CONFIG_OBJ=\
	liboddcast_wxconfig.o


libtranscoder_a_SOURCES=\
	cbuffer.h \
	mp3decoder.h \
	oggvdecoder.h \
	resample.h \
	Socket.h \
	transcode.h \
	transcurl.h \
	cbuffer.cpp \
	mp3decoder.cpp \
	oggvdecoder.cpp \
	transcurl.cpp \
	transcode.cpp

LIBTRANSCODER_OBJ=\
	cbuffer.o \
	mp3decoder.o \
	oggvdecoder.o \
	transcurl.o \
	transcode.o

liboddcast_a_SOURCES=\
	log.h \
	liboddcast.h \
	log.cpp \
	Socket.cpp \
	liboddcast.cpp \
	resample.c

LIBODDCASTOBJ=\
	log.o \
	Socket.o \
	liboddcast.o \
	resample.o

streamTranscoder_CL_SOURCES=\
			commandline.cpp

streamTranscoder_NUI_SOURCES=\
			ncursesUI.cpp\
			transcode_nui.cpp

streamTranscoder_WX_SOURCES=\
	streamTranscoder_WX.cpp

SUFFIXES=.cpp .o
CPPFLAGS=@VORBIS_CFLAGS@ @OGG_CFLAGS@ @CURL_CFLAGS@ @MAD_CFLAGS@ @LAME_CFLAGS@ -I. @WXFLAGS@
LFLAGS=-L. -loddcast -ltranscoder @VORBISFILE_LIBS@ @VORBISENC_LIBS@ @VORBIS_LIBS@ @OGG_LIBS@ @CURL_LIBS@ @MAD_LIBS@ @LAME_LIBS@  -lpthread 
LIBODDCAST=liboddcast.a
LIBTRANSCODER=libtranscoder.a

bin_PROGRAMS= liboddcast.a libtranscoder.a streamTranscoder_CL
EXTRA_PROGRAMS= liboddcast_config.a streamTranscoder_WX streamTranscoder_NUI

wx_LDDADD= -loddcast_config `wx-config --libs`

liboddcast_config.a: $(LIBODDCAST_CONFIG_OBJ)
	ar cr liboddcast_config.a $(LIBODDCAST_CONFIG_OBJ)

liboddcast.a: $(LIBODDCASTOBJ)
	ar cr liboddcast.a $(LIBODDCASTOBJ)

libtranscoder.a: $(LIBTRANSCODER_OBJ)
	ar cr libtranscoder.a $(LIBTRANSCODER_OBJ)

streamTranscoder_CL: commandline.cpp $(OBJ)
	$(CXX) $(CPPFLAGS) -o streamTranscoder_CL commandline.cpp $(OBJ) $(LFLAGS)

streamTranscoder_NUI: ncursesUI.cpp $(OBJ)
	$(CXX) $(CPPFLAGS) -o streamTranscoder_NUI ncursesUI.cpp $(OBJ) $(LFLAGS) -lncurses

streamTranscoder_WX: liboddcast_config.a
	$(CXX) $(CPPFLAGS) `wx-config --cflags` -o streamTranscoder_WX streamTranscoder_WX.cpp $(LFLAGS) `wx-config --libs` -loddcast_config

finished:
	@echo "----------------------------------------------------------------------"
	@echo "Command Line streamTranscoder and nCurses streamTranscoder built!"
	@echo ""
	@echo "   - command line = streamTranscoder_CL"
	@echo "   - ncurses ui   = streamTranscoder_NUI"
	@echo ""
	@echo "----------------------------------------------------------------------"

clean:
	rm -rf transcode *.o $(LIBODDCAST) $(LIBTRANSCODER) $(LIBODDCASTOBJ) $(LIBODDCASTOBJ) $(LIBODDCAST_CONFIG_OBJ) streamTranscoder_NUI wxstreamTranscoder streamTranscoder_CL

