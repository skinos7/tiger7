### Auto name
PWD_DIR := $(shell pwd)
BINS := $(shell basename ${PWD_DIR})
DYNAMIC_LIB := ${BINS}.com

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
		  -DPROJECT_ID=\"${PROJECT_ID}\" \
		  -DCOM_ID=\"${BINS}\" \
		  -DCOM_IDPATH=\"${PROJECT_ID}@${BINS}\"
CFLAGS += ${gCOM_CFLAGS}
LDFLAGS += ${gCOM_LDFLAGS}

### Auto c file
SRCS := $(wildcard *.c)
OBJS := $(subst .c,.o,$(SRCS))
sinclude mconfig

### Common Actions
all: ${DYNAMIC_LIB}
install:
clean distclean:
	@rm -f *.[oa] *.com *.elf *.so *~ *.gdb ${DYNAMIC_LIB} ${OBJS}
${DYNAMIC_LIB}: ${OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ ${LIBSO_ADDIN} 
${OBJS}:%.o:%.c
	${CC} -c ${CFLAGS} -fPIC $< -o $@
.PHONY: all install clean distclean

