
EMUGEN := ./emugen

.PHONY: build clean

build:
	$(MAKE) -C emulator/opengl/host/tools/emugen
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
	$(MAKE) -C libs/utils
	$(MAKE) -C emulator/opengl/shared/OpenglOsUtils
	$(MAKE) -C emulator/opengl/shared/OpenglCodecCommon -f Makefile.krnlyng
	$(MAKE) -C emulator/opengl/host/libs/GLESv1_dec
	$(MAKE) -C emulator/opengl/host/libs/GLESv2_dec
	$(MAKE) -C emulator/opengl/host/libs/renderControl_dec
#	$(MAKE) -C emulator/opengl/host/libs/Translator/EGL
#	$(MAKE) -C emulator/opengl/host/libs/Translator/GLES_CM
#	$(MAKE) -C emulator/opengl/host/libs/Translator/GLES_V2
	$(MAKE) -C emulator/opengl/host/libs/libOpenglRender
#	$(MAKE) -C emulator/opengl/host/renderer
	$(MAKE) -C emulator/opengl/tests/emulator_test_renderer

clean:
	$(MAKE) -C emulator/opengl/host/tools/emugen clean
	rm -rf lib_renderControl_dec_intermediates
	rm -rf libGLESv2_dec_intermediates
	rm -rf libGLESv1_dec_intermediates
#	rm -rf lib_renderControl_enc_intermediates
#	rm -rf libGLESv2_enc_intermediates
#	rm -rf libGLESv1_enc_intermediates
	$(MAKE) -C libs/utils clean
	$(MAKE) -C emulator/opengl/shared/OpenglOsUtils clean
	$(MAKE) -C emulator/opengl/shared/OpenglCodecCommon -f Makefile.krnlyng clean
	$(MAKE) -C emulator/opengl/host/libs/GLESv1_dec clean
	$(MAKE) -C emulator/opengl/host/libs/GLESv2_dec clean
	$(MAKE) -C emulator/opengl/host/libs/renderControl_dec clean
#	$(MAKE) -C emulator/opengl/host/libs/Translator/EGL clean
#	$(MAKE) -C emulator/opengl/host/libs/Translator/GLES_CM clean
#	$(MAKE) -C emulator/opengl/host/libs/Translator/GLES_V2 clean
	$(MAKE) -C emulator/opengl/host/libs/libOpenglRender clean
#	$(MAKE) -C emulator/opengl/host/renderer clean
	$(MAKE) -C emulator/opengl/tests/emulator_test_renderer clean

