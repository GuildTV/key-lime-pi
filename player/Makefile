include Makefile.include

CFLAGS+=-std=c++0x -DDEBUGLOG -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_CMAKE_CONFIG -D__VIDEOCORE4__ -U_FORTIFY_SOURCE -Wall -DHAVE_OMXLIB -DUSE_EXTERNAL_FFMPEG  -DHAVE_LIBAVCODEC_AVCODEC_H -DHAVE_LIBAVUTIL_OPT_H -DHAVE_LIBAVUTIL_MEM_H -DHAVE_LIBAVUTIL_AVUTIL_H -DHAVE_LIBAVFORMAT_AVFORMAT_H -DHAVE_LIBAVFILTER_AVFILTER_H -DOMX -DOMX_SKIP64BIT -ftree-vectorize -DUSE_EXTERNAL_OMX -DTARGET_RASPBERRY_PI -DUSE_EXTERNAL_LIBBCM_HOST

LDFLAGS+=-L./ -lc -lWFC -lGLESv2 -lEGL -lbcm_host -lopenmaxil -ljsoncpp -lz -lfreetype -l bcm2835
INCLUDES+=-I./ -Ilinux -Iffmpeg_compiled/include/ -I/opt/vc/include. -I/usr/include/freetype2

SRCSLAVE=linux/XMemUtils.cpp \
		utils/log.cpp \
		OMX/DynamicDll.cpp \
		utils/RegExp.cpp \
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
		LimeSlave.cpp \
		LimeTimer.cpp \

OBJSSLAVE+=$(filter %.s.o,$(SRCSLAVE:.cpp=.s.o))

SRCMASTER=linux/XMemUtils.cpp \
		utils/log.cpp \
		OMX/DynamicDll.cpp \
		utils/RegExp.cpp \
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
		LimeMaster.cpp \
		LimeTimer.cpp \

OBJSMASTER+=$(filter %.m.o,$(SRCMASTER:.cpp=.m.o))

SRCNETTEST=net/NetIO.cpp \
		net/NetUser.cpp \
		net/NetMessageQueue.cpp \
		logger.cpp \
		test/NetTest.cpp \

OBJSNETTEST+=$(filter %.o,$(SRCNETTEST:.cpp=.o))

SRCGPIOTEST=GPIO.cpp \
		logger.cpp \
		test/GPIOtest.cpp \

OBJSGPIOTEST+=$(filter %.o,$(SRCGPIOTEST:.cpp=.o))

all: master.bin

%.m.o: %.cpp
	@rm -f $@
	$(CXX) $(CFLAGS) -DLIMEMASTER $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.s.o: %.cpp
	@rm -f $@
	$(CXX) $(CFLAGS) -DLIMESLAVE $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

list_test:
	$(CXX) -O3 -o list_test list_test.cpp

slave.bin: $(OBJSSLAVE)
	$(CXX) $(LDFLAGS) -o slave.bin $(OBJSSLAVE) -lvchiq_arm -lvcos -lrt -lpthread -lavutil -lavcodec -lavformat -lswscale -lpcre -l bcm2835
	#arm-unknown-linux-gnueabi-strip slave.bin

master.bin: $(OBJSMASTER)
	$(CXX) $(LDFLAGS) -o master.bin $(OBJSMASTER) -lvchiq_arm -lvcos -lrt -lpthread -lavutil -lavcodec -lavformat -lswscale -lpcre -l bcm2835
	#arm-unknown-linux-gnueabi-strip master.bin

nettest.bin: $(OBJSNETTEST)
	$(CXX) $(LDFLAGS) -o nettest.bin $(OBJSNETTEST) -lvchiq_arm -lvcos -lrt -lpthread
	#arm-unknown-linux-gnueabi-strip nettest.bin

gpiotest.bin: $(OBJSGPIOTEST)
	$(CXX) $(LDFLAGS) -o gpiotest.bin $(OBJSGPIOTEST) -lvchiq_arm -lvcos -lrt -lpthread -DGPIOTEST
	#arm-unknown-linux-gnueabi-strip gpiotest.bin

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f omxplayer.old.log omxplayer.log
	@rm -f omxplayer.bin
	@rm -rf omxplayer-dist
	@rm -f omxplayer-dist.tar.gz

dist: omxplayer.bin
	mkdir -p omxplayer-dist/usr/lib/omxplayer
	mkdir -p omxplayer-dist/usr/usr/bin
	mkdir -p omxplayer-dist/usr/share/doc
	cp omxplayer omxplayer.bin omxplayer-dist/usr/usr/bin
	cp COPYING omxplayer-dist/usr/share/doc/
	cp README.md omxplayer-dist/usr/share/doc/README
	cp -a ffmpeg_compiled/usr/local/lib/*.so* omxplayer-dist/usr/lib/omxplayer/
	tar -czf omxplayer-dist.tar.gz omxplayer-dist

