SUBDIRS  := core interpose stack test

include $(foreach subdir,$(SUBDIRS),$(wildcard $(SRCDIR)/$(subdir)/module.mk))
