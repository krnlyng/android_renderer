OUT         := ../../../../../libOpenglRender.so
SRC         := 	ColorBuffer.cpp \
				NativeLinuxSubWindow.cpp \
				RenderThread.cpp \
				EGLDispatch.cpp \
				ThreadInfo.cpp \
				FBConfig.cpp \
				ReadBuffer.cpp \
				WindowSurface.cpp \
				FrameBuffer.cpp \
				RenderContext.cpp \
				render_api.cpp \
				GL2Dispatch.cpp \
				RenderControl.cpp \
				GLDispatch.cpp \
				RenderServer.cpp
OBJ         := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
OBJ         += $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))
DEP         := $(OBJ:.o=.d)

ARM			?= 1

CFLAGS      := -Wall -fPIC -DHAVE_PTHREADS -DWITH_GLES2 -fpermissive
CXXFLAGS    := -Wall -fPIC -DHAVE_PTHREADS -DWITH_GLES2 -fpermissive
LDFLAGS     := -L../../../../..
LDLIBS      := -lOpenglCodecCommon

ifeq ($(ARM),1)
	CFLAGS += -marm
	CXXFLAGS += -marm
endif

CFLAGS		+= -I../../include -I../../../shared/OpenglOsUtils/ -I../../../host/include/libOpenglRender/ -I../../../shared/OpenglCodecCommon -I../../../host/libs/GLESv1_dec -I../../../../../libGLESv1_dec_intermediates -I../../../../../emulator/opengl/system/GLESv1_enc -I../../../../../emulator/opengl/host/libs/GLESv2_dec -I../../../../../libGLESv2_dec_intermediates -I../../../../../emulator/opengl/system/GLESv2_enc -I../../../../../lib_renderControl_dec_intermediates -I../../../../../include -I../../../../../emulator/opengl/system/renderControl_enc
CXXFLAGS	+= -I../../include -I../../../shared/OpenglOsUtils/ -I../../../host/include/libOpenglRender/ -I../../../shared/OpenglCodecCommon -I../../../host/libs/GLESv1_dec -I../../../../../libGLESv1_dec_intermediates -I../../../../../emulator/opengl/system/GLESv1_enc -I../../../../../emulator/opengl/host/libs/GLESv2_dec -I../../../../../libGLESv2_dec_intermediates -I../../../../../emulator/opengl/system/GLESv2_enc -I../../../../../lib_renderControl_dec_intermediates -I../../../../../include -I../../../../../emulator/opengl/system/renderControl_enc

DEBUG       ?= 0
VERBOSE     ?= 0
X11			?= 0
DEFINE_GLCHAR	?= 0

ifeq ($(DEBUG),1)
	CFLAGS += -O0 -g3 -ggdb -pg
	CXXFLAGS += -O0 -g3 -ggdb -pg
	LDFLAGS += -pg
endif

ifeq ($(VERBOSE),1)
	MSG := @true
	CMD :=
else
	MSG := @echo
	CMD := @
endif

ifeq ($(X11),1)
	CFLAGS += -DX11
	CXXFLAGS += -DX11
	LDLIBS += -lX11
else
	LDLIBS += `pkg-config --libs wayland-egl` `pkg-config --libs wayland-client`
endif

ifeq ($(DEFINE_GLCHAR),1)
	CFLAGS += -DDEFINE_GLCHAR=1
	CXXFLAGS += -DDEFINE_GLCHAR=1
endif

.PHONY: release clean

release: CFLAGS += -O3
release: CXXFLAGS += -O3
release: $(OUT)

clean:
	$(MSG) -e "\tCLEAN\t"
	$(CMD)$(RM) $(OBJ) $(DEP) $(OUT)

$(OUT): $(OBJ)
	$(MSG) -e "\tLINK\t$@"
	$(CMD)$(CXX) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS)

%.o: %.c %.d
	$(MSG) -e "\tCC\t$@"
	$(CMD)$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	$(MSG) -e "\tDEP\t$@"
	$(CMD)$(CC) $(CFLAGS) -MF $@ -MM $<

%.o: %.cpp %.d
	$(MSG) -e "\tCXX\t$@"
	$(CMD)$(CXX) $(CXXFLAGS) -c $< -o $@

%.d: %.cpp
	$(MSG) -e "\tDEP\t$@"
	$(CMD)$(CXX) $(CXXFLAGS) -MF $@ -MM $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif

