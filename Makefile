CC = gcc  
CFLAGS = -O2 -Wall  

define make_subdir
	@for subdir in $(SUBDIRS) ; do \
		(cd $$subdir && make $1) \
	done;
endef

SUBDIRS = debug_lib lib client server
.PHONY:$(SUBDIRS) clean

all:$(SUBDIRS)

$(SUBDIRS):
	${MAKE} -C $@ 

clean:
	$(call make_subdir , clean) 
