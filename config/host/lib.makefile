### Auto name
PWD_DIR := $(shell pwd)
BINS := $(shell basename ${PWD_DIR})
STATIC_LIB ?= lib${BINS}.a
DYNAMIC_LIB := lib${BINS}.so
DYNAMIC_LIB_VERSION=0

### Add the xflags define and library include
CFLAGS += -s -O2 -Wall -Werror
CFLAGS += -DgPLATFORM=\"${gPLATFORM}\" \
		  -DgHARDWARE=\"${gHARDWARE}\" \
		  -DgCUSTOM=\"${gCUSTOM}\" \
		  -DgSCOPE=\"${gSCOPE}\" \
		  -DgVERSION=\"${gVERSION}\" \
		  -DgMAKER=\"${gMAKER}\" \
		  -DgPLATFORM__${gPLATFORM} \
		  -DgHARDWARE__${gHARDWARE} \
		  -DgCUSTOM__${gCUSTOM} \
		  -DgSCOPE__${gSCOPE} \
		  -DPROJECT_ID=\"${PROJECT_ID}\"
CFLAGS += ${gLIB_CFLAGS}
LDFLAGS += ${gLIB_LDFLAGS}

### Auto c file
HDS := $(wildcard *.h)
SRCS := $(wildcard *.c)
OBJS := $(subst .c,.o,$(SRCS))
sinclude mconfig

### Common Actions
all: ${STATIC_LIB} ${DYNAMIC_LIB}
install:
	install -d ${gINSTALL_DIR}/include/${BINS}
	cp $(HDS) ${gINSTALL_DIR}/include/${BINS}
	cp ${DYNAMIC_LIB} ${gINSTALL_DIR}/lib
	ln -sf ${DYNAMIC_LIB} ${gINSTALL_DIR}/lib/${DYNAMIC_LIB}.${DYNAMIC_LIB_VERSION}
clean distclean:
	@rm -f *.[oa] *.elf *.so *~ *.gdb ${DYNAMIC_LIB} ${STATIC_LIB} ${OBJS}
${STATIC_LIB}: ${OBJS}
	${AR} rcs $@ $^ ${LIBA_ADDIN}
${DYNAMIC_LIB}: ${OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ ${LIBSO_ADDIN} 
${OBJS}:%.o:%.c
	${CC} -c ${CFLAGS} -fPIC $< -o $@
.PHONY: all install clean distclean

