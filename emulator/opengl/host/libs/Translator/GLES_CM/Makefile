OUT         := ../../../../../../libGLES_CM_translator.so
SRC         :=	GLEScmContext.cpp \
				GLEScmImp.cpp \
				GLEScmUtils.cpp \
				GLEScmValidate.cpp \
				../GLcommon/DummyGLfuncs.cpp \
				../GLcommon/FramebufferData.cpp \
				../GLcommon/GLESbuffer.cpp \
				../GLcommon/GLDispatch.cpp \
				../GLcommon/GLEScontext.cpp \
				../GLcommon/GLESpointer.cpp \
				../GLcommon/GLESvalidate.cpp \
				../GLcommon/GLutils.cpp \
				../GLcommon/PaletteTexture.cpp \
				../GLcommon/RangeManip.cpp \
				../GLcommon/TextureUtils.cpp \
				../GLcommon/etc1.cpp \
				../GLcommon/objectNameManager.cpp

OBJ         := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
OBJ         += $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))
DEP         := $(OBJ:.o=.d)

CFLAGS      := -Wall -fPIC -DHAVE_PTHREADS -DWITH_GLES2 -fpermissive
CXXFLAGS    := -Wall -fPIC -DHAVE_PTHREADS -DWITH_GLES2 -fpermissive
LDFLAGS     := -L../../../../../../
LDLIBS      := -lutils -lOpenglOsUtils

CFLAGS		+= -I../../../../../../include -I../../../../../../emulator/opengl/shared -I../include
CXXFLAGS	+= -I../../../../../../include -I../../../../../../emulator/opengl/shared -I../include

DEBUG       ?= 0
VERBOSE     ?= 0

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

