IDIR =inc
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=bin

_DEPS = aes.h bytesub.h keyexpand.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = bytesub.o keyexpand.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

aes: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ CC = gcc
