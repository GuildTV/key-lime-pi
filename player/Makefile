include Makefile.include

CFLAGS+=-std=c++0x -DDEBUGLOG -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_CMAKE_CONFIG -D__VIDEOCORE4__ -U_FORTIFY_SOURCE -Wall -DHAVE_OMXLIB -DUSE_EXTERNAL_FFMPEG  -DHAVE_LIBAVCODEC_AVCODEC_H -DHAVE_LIBAVUTIL_OPT_H -DHAVE_LIBAVUTIL_MEM_H -DHAVE_LIBAVUTIL_AVUTIL_H -DHAVE_LIBAVFORMAT_AVFORMAT_H -DHAVE_LIBAVFILTER_AVFILTER_H -DOMX -DOMX_SKIP64BIT -ftree-vectorize -DUSE_EXTERNAL_OMX -DTARGET_RASPBERRY_PI -DUSE_EXTERNAL_LIBBCM_HOST

LDFLAGS+=-L./ -lc -lWFC -lGLESv2 -lEGL -lbcm_host -lopenmaxil -ljsoncpp -lz -lfreetype -l bcm2835
INCLUDES+=-I./ -Ilinux -Iffmpeg_compiled/include/ -I/opt/vc/include. -I/usr/include/freetype2

SRC=linux/XMemUtils.cpp \
		utils/log.cpp \
		OMX/DynamicDll.cpp \
		OMX/BitstreamConverter.cpp \
		render/Freetype.cpp \
		render/OverlayRenderer.cpp \
		linux/RBP.cpp \
		OMX/OMXThread.cpp \
		OMX/OMXReader.cpp \
		OMX/OMXStreamInfo.cpp \
		OMX/OMXCore.cpp \
		OMX/OMXVideo.cpp \
		OMX/OMXClock.cpp \
		render/MyRender.cpp \
		OMX/File.cpp \
		OMX/OMXPlayerVideo.cpp \
		OMXLink.cpp \
		logger.cpp \
		net/NetIO.cpp \
		net/NetMessageQueue.cpp \
		net/NetUser.cpp \
		OMXWrapper.cpp \
		LimeTimer.cpp \
		LimeShared.cpp \

OBJSMASTER+=$(filter %.m.o,$(SRC:.cpp=.m.o))
OBJSSLAVE+=$(filter %.s.o,$(SRC:.cpp=.s.o))

SRCNETTEST=net/NetIO.cpp \
		net/NetUser.cpp \
		net/NetMessageQueue.cpp \
		logger.cpp \
		test/NetTest.cpp \

OBJSNETTEST+=$(filter %.o,$(SRCNETTEST:.cpp=.o))

all: nettest.bin master.bin slave.bin

%.m.o: %.cpp
	@rm -f $@
	$(CXX) $(CFLAGS) -DLIMEMASTER $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.s.o: %.cpp
	@rm -f $@
	$(CXX) $(CFLAGS) -DLIMESLAVE $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.o: %.cpp
	@rm -f $@
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

slave.bin: $(OBJSSLAVE) LimeSlave.s.o
	$(CXX) $(LDFLAGS) -o slave.bin $(OBJSSLAVE) LimeSlave.s.o -lvchiq_arm -lvcos -lrt -lpthread -lavutil -lavcodec -lavformat -lswscale -lpcre -l bcm2835
	#arm-unknown-linux-gnueabi-strip slave.bin

master.bin: $(OBJSMASTER) LimeMasterDownStream.m.o LimeMaster.m.o
	$(CXX) $(LDFLAGS) -o master.bin $(OBJSMASTER) LimeMasterDownStream.m.o LimeMaster.m.o -lvchiq_arm -lvcos -lrt -lpthread -lavutil -lavcodec -lavformat -lswscale -lpcre -l bcm2835
	#arm-unknown-linux-gnueabi-strip master.bin

nettest.bin: $(OBJSNETTEST)
	$(CXX) $(LDFLAGS) -o nettest.bin $(OBJSNETTEST) -lvchiq_arm -lvcos -lrt -lpthread
	#arm-unknown-linux-gnueabi-strip nettest.bin
