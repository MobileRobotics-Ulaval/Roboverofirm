#
# Robovero samples/lib Makefile
# David St-Onge 04/2014
#
.PHONY: clean

# Cross compiler
CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++

SRCS	= $(wildcard *.c)
OBJS	= $(patsubst %.c,%.o,${SRCS})
#LIBS	= -lpthread -lm

CFLAGS = -O2 -std=c99 -D_XOPEN_SOURCE=700 -D_BSD_SOURCE
CXXFLAGS = -O2

# for debugging
#CFLAGS 	= -g

TARGET	= robocomm

.PHONY: all clean depend

robocomm: robocomm.o
	@echo "Building $@"
	@$(CC) -o $@ $? $(LIBS) $(INCS) $(CFLAGS)
	
all: 	${TARGET}

clean : 
	@echo "Cleaning"
	@rm -f *.o .depend ${TARGET} *~

depend:	
	@echo "Building dependencies"
	@rm -f .depend
	@touch .depend
	@makedepend ${SYS_INCLUDES} ${INCS} -Y -f .depend ${SRCS}

%.o:	%.c
	@echo "Compiling $@"
	$(CC) $(INCS) -c $(CFLAGS) $< -o $@

ifeq (.depend,$(wildcard .depend))
include .depend 
endif
