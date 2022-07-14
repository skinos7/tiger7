
### Auto c file
DIRSRCS := $(wildcard ${SUBDIRS}/*.c)
SRCS := $(notdir $(DIRSRCS))
OBJS := $(subst .c,.o,$(SRCS))

### Add the C flags
EXTRA_CFLAGS +=

### Make the signal .c to ko file
#obj-m := ${OBJS}

### Make the all .c to ko file
mod-objs := ${OBJS}
obj-m := mod.o

