OUT         := ../../libutils.so
SRC         := 	abort_socket.c \
				android_reboot.c \
				array.c \
				ashmem-host.c \
				atomic.c \
				buffer.c \
				config_utils.c \
				cpu_info.c \
				debugger.c \
				dir_hash.c \
				dlmalloc_stubs.c \
				hashmap.c \
				iosched_policy.c \
				klog.c \
				list.c \
				load_file.c \
				memory.c \
				mq.c \
				mspace.c \
				native_handle.c \
				open_memstream.c \
				partition_utils.c \
				process_name.c \
				properties.c \
				qsort_r_compat.c \
				qtaguid.c \
				record_stream.c \
				sched_policy.c \
				selector.c \
				socket_inaddr_any_server.c \
				socket_local_client.c \
				socket_local_server.c \
				socket_loopback_client.c \
				socket_loopback_server.c \
				socket_network_client.c \
				sockets.c \
				str_parms.c \
				strdup16to8.c \
				strdup8to16.c \
				threads.c \
				tzstrftime.c \
				tztime.c \
				uevent.c \
				uio.c \
				zygote.c
#				ashmem-dev.c
#				atomic-android-sh.c
OBJ         := $(patsubst %.c, %.o, $(filter %.c, $(SRC)))
OBJ         += $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))
DEP         := $(OBJ:.o=.d)

SRC_DEPENDS :=

CFLAGS      := -Wall -D__USE_GNU -fPIC -DHAVE_PTHREADS -DWITH_GLES2
CXXFLAGS    := -Wall -fPIC -DHAVE_PTHREADS -DWITH_GLES2
LDFLAGS     :=
LDLIBS      :=

CFLAGS		+= -I../../include -I../../libs/libc/include -DHAVE_OFF64_T -DHAVE_SYS_UIO_H -DHAVE_ENDIAN_H -DHAVE_POSIX_FILEMAP -DANDROID_SMP=0
CXXFLAGS	+= -I../../include -I../../libs/libc/include -DHAVE_OFF64_T -DHAVE_SYS_UIO_H -DHAVE_ENDIAN_H -DHAVE_POSIX_FILEMAP -DANDROID_SMP=0

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

%.o: %.c %.d $(SRC_DEPENDS)
	$(MSG) -e "\tCC\t$@"
	$(CMD)$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c $(SRC_DEPENDS)
	$(MSG) -e "\tDEP\t$@"
	$(CMD)$(CC) $(CFLAGS) -MF $@ -MM $<

%.o: %.cpp %.d $(SRC_DEPENDS)
	$(MSG) -e "\tCXX\t$@"
	$(CMD)$(CXX) $(CXXFLAGS) -c $< -o $@

%.d: %.cpp $(SRC_DEPENDS)
	$(MSG) -e "\tDEP\t$@"
	$(CMD)$(CXX) $(CXXFLAGS) -MF $@ -MM $<

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif

