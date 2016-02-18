CC=gcc

INCDIR=inc
OUTDIR=bin
SRCDIR=src

CFLAGS=-I$(IDIR)

_DEPS = aes.h bytesub.h keyexpand.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = bytesub.o keyexpand.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

MAIN=aes.c

all: aes  

aes: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ CC = gcc
