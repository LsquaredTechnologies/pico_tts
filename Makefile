TOPTARGETS := all clean

SUBDIRS := ./libsvoxpico ./pico2wave

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)