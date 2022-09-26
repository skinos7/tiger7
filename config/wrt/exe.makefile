### Auto name
PWD_DIR := $(shell pwd)
BINS := $(shell basename ${PWD_DIR})

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
		  -DCOM_IDPATH=\"${PROJECT_ID}@${BINS}\" \
		  -DEXE_ID=\"${BINS}\" \
		  -DEXE_IDPATH=\"${PROJECT_ID}@${BINS}\"
CFLAGS += ${gEXE_CFLAGS}
LDFLAGS += ${gEXE_LDFLAGS}

### Auto c file
SRCS := $(wildcard ./*.c)
OBJS := $(subst .c,.o,$(SRCS))
sinclude mconfig

### Common Actions
all: ${BINS}
install:
clean distclean:
	@rm -f *.[oa] *.elf *.so *~ core ${BINS} *.gdb
${BINS}: ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}
${OBJS}:%.o:%.c
	${CC} ${CFLAGS} -c $< -o $@
	if [ "X${STRIP}" != "X" ]; then \
		${STRIP} -x -R .note -R .comment $@; \
	fi
.PHONY: all install clean distclean

