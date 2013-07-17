MODULE := engines/twin

MODULE_OBJS := \
	detection.o \
	fla_decoder.o \
	gfx_opengl.o \
	grid.o \
	hqr.o \
	image.o \
	island.o \
	lzss.o \
	scene.o \
	twin.o \
	video_player.o \
	video_player_fla.o \
	video_player_smk.o 

# This module can be built as a plugin
ifeq ($(ENABLE_TWIN), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
