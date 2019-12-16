TARGET:=pikeyd165

#comment out the line below when _NOT_ cross compiling
#include crosscomp.rules

ifneq ($(CROSS), yes)
CC := gcc
LDFLAGS :=
INCLUDE :=
endif

CFLAGS=-O2 -Wstrict-prototypes -Wmissing-prototypes $(INCLUDE)
CFLAGS=-O3 -funroll-loops -Wstrict-prototypes -Wmissing-prototypes $(INCLUDE)
SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.o,$(SRC))
LIBS=-lbcm2835


#get build dir and date
SHELL := /bin/bash
VPATH := $(shell basename `pwd`)
VDATE := $(shell date)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -DBVERSION='"$(VPATH)"' -DBDATE='"$(VDATE)"' -s $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -DBVERSION='"$(VPATH)"' -DBDATE='"$(VDATE)"'  $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o *~


