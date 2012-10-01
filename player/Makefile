
CFLAGS:=-isystem$(PREFIX)/include -pipe -mfloat-abi=hard -mcpu=arm1176jzf-s -fomit-frame-pointer -mabi=aapcs-linux -mtune=arm1176jzf-s -mfpu=vfp -Wno-psabi -mno-apcs-stack-check -O3 -mstructure-size-boundary=32 -mno-sched-prolog -std=c++0x -DDEBUGLOG -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_CMAKE_CONFIG -D__VIDEOCORE4__ -U_FORTIFY_SOURCE -Wall -DHAVE_OMXLIB -DUSE_EXTERNAL_FFMPEG  -DHAVE_LIBAVCODEC_AVCODEC_H -DHAVE_LIBAVUTIL_OPT_H -DHAVE_LIBAVUTIL_MEM_H -DHAVE_LIBAVUTIL_AVUTIL_H -DHAVE_LIBAVFORMAT_AVFORMAT_H -DHAVE_LIBAVFILTER_AVFILTER_H -DOMX -DOMX_SKIP64BIT -ftree-vectorize -DUSE_EXTERNAL_OMX -DTARGET_RASPBERRY_PI -DUSE_EXTERNAL_LIBBCM_HOST

LDFLAGS:=-L/lib -L/usr/lib -L/opt/vc/lib/ -L./ -lc -lWFC -lGLESv2 -lEGL -lbcm_host -lopenmaxil -ljsoncpp -lz -lfreetype -l bcm2835 -lpng
INCLUDES:=-isystem/opt/vc/include -isystem/usr/include -isystem/opt/vc/include/interface/vcos/pthreads -I./ -Ilinux -Iffmpeg_compiled/include/ -I/opt/vc/include. -I/usr/include/freetype2

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
		render/TextTexture.cpp \
		render/TextureRender.cpp \
		render/ColourTexture.cpp \
        render/FadeTexture.cpp \
        render/MaskTexture.cpp \
        render/SolidTexture.cpp \
        render/PNGTexture.cpp \
		render/EffectParser.cpp \
		OMX/File.cpp \
		OMX/OMXPlayerVideo.cpp \
		OMXLink.cpp \
		logger.cpp \
		net/NetIO.cpp \
		net/NetMessageQueue.cpp \
		net/NetUser.cpp \
		OMXWrapper.cpp \
		JsonUtil.cpp \
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
	g++ $(CFLAGS) -DLIMEMASTER $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.s.o: %.cpp
	@rm -f $@
	g++ $(CFLAGS) -DLIMESLAVE $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.o: %.cpp
	@rm -f $@
	g++ $(CFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

slave.bin: $(OBJSSLAVE) LimeSlave.s.o
	g++ $(LDFLAGS) -o slave.bin $(OBJSSLAVE) LimeSlave.s.o -lvchiq_arm -lvcos -lrt -lpthread -lavutil -lavcodec -lavformat -lswscale -lpcre -l bcm2835
	#arm-unknown-linux-gnueabi-strip slave.bin

master.bin: $(OBJSMASTER) LimeMasterDownStream.m.o LimeMaster.m.o
	g++ $(LDFLAGS) -o master.bin $(OBJSMASTER) LimeMasterDownStream.m.o LimeMaster.m.o -lvchiq_arm -lvcos -lrt -lpthread -lavutil -lavcodec -lavformat -lswscale -lpcre -l bcm2835
	#arm-unknown-linux-gnueabi-strip master.bin

nettest.bin: $(OBJSNETTEST)
	g++ $(LDFLAGS) -o nettest.bin $(OBJSNETTEST) -lvchiq_arm -lvcos -lrt -lpthread
	#arm-unknown-linux-gnueabi-strip nettest.bin
