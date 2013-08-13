MODULE := engines/twin

MODULE_OBJS := \
	actor.o \
	animation.o \
	block.o \
	block_library.o \
	colour_palette.o \
	detection.o \
	entity_information.o \
	fla_decoder.o \
	gfx_opengl.o \
	grid.o \
	hqr.o \
	image.o \
	island.o \
	lzss.o \
	model.o \
	player.o \
	resource.o \
	scene.o \
	script.o \
	script_life_v2.o \
	script_track_v2.o \
	sprite.o \
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
