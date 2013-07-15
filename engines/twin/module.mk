MODULE := engines/twin

MODULE_OBJS := \
	detection.p \
	twin.o

# This module can be built as a plugin
ifeq ($(ENABLE_TWIN), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
