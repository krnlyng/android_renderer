
EMUGEN := ./emugen

.PHONY: build clean

build:
	make -C emulator/opengl/host/tools/emugen
ifeq ("$(wildcard lib_renderControl_dec_intermediates)", "")
	mkdir -p lib_renderControl_dec_intermediates
	$(EMUGEN) -D lib_renderControl_dec_intermediates -i emulator/opengl/system/renderControl_enc renderControl
endif
ifeq ("$(wildcard libGLESv2_dec_intermediates)", "")
	mkdir -p libGLESv2_dec_intermediates
	$(EMUGEN) -D libGLESv2_dec_intermediates -i emulator/opengl/system/GLESv2_enc gl2
endif
ifeq ("$(wildcard libGLESv1_dec_intermediates)", "")
	mkdir -p libGLESv1_dec_intermediates
	$(EMUGEN) -D libGLESv1_dec_intermediates -i emulator/opengl/system/GLESv1_enc gl
endif
#ifeq ("$(wildcard lib_renderControl_enc_intermediates)", "")
#	mkdir -p lib_renderControl_enc_intermediates
#	$(EMUGEN) -E lib_renderControl_enc_intermediates -i emulator/opengl/system/renderControl_enc renderControl
#endif
#ifeq ("$(wildcard libGLESv2_enc_intermediates)", "")
#	mkdir -p libGLESv2_enc_intermediates
#	$(EMUGEN) -E libGLESv2_enc_intermediates -i emulator/opengl/system/GLESv2_enc gl2
#endif
#ifeq ("$(wildcard libGLESv1_enc_intermediates)", "")
#	mkdir -p libGLESv1_enc_intermediates
#	$(EMUGEN) -E libGLESv1_enc_intermediates -i emulator/opengl/system/GLESv1_enc gl
#endif
	make -C libs/utils
	make -C emulator/opengl/shared/OpenglOsUtils
	make -C emulator/opengl/shared/OpenglCodecCommon -f Makefile.krnlyng
	make -C emulator/opengl/host/libs/GLESv1_dec
	make -C emulator/opengl/host/libs/GLESv2_dec
	make -C emulator/opengl/host/libs/renderControl_dec
#	make -C emulator/opengl/host/libs/Translator/EGL
#	make -C emulator/opengl/host/libs/Translator/GLES_CM
#	make -C emulator/opengl/host/libs/Translator/GLES_V2
	make -C emulator/opengl/host/libs/libOpenglRender
	make -C emulator/opengl/host/renderer
	make -C emulator/opengl/tests/emulator_test_renderer

clean:
	make -C emulator/opengl/host/tools/emugen clean
	rm -rf lib_renderControl_dec_intermediates
	rm -rf libGLESv2_dec_intermediates
	rm -rf libGLESv1_dec_intermediates
#	rm -rf lib_renderControl_enc_intermediates
#	rm -rf libGLESv2_enc_intermediates
#	rm -rf libGLESv1_enc_intermediates
	make -C libs/utils clean
	make -C emulator/opengl/shared/OpenglOsUtils clean
	make -C emulator/opengl/shared/OpenglCodecCommon -f Makefile.krnlyng clean
	make -C emulator/opengl/host/libs/GLESv1_dec clean
	make -C emulator/opengl/host/libs/GLESv2_dec clean
	make -C emulator/opengl/host/libs/renderControl_dec clean
#	make -C emulator/opengl/host/libs/Translator/EGL clean
#	make -C emulator/opengl/host/libs/Translator/GLES_CM clean
#	make -C emulator/opengl/host/libs/Translator/GLES_V2 clean
	make -C emulator/opengl/host/libs/libOpenglRender clean
	make -C emulator/opengl/host/renderer clean
	make -C emulator/opengl/tests/emulator_test_renderer clean

