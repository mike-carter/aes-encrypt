#
# 'make depend'	uses makedepend to automatically generate dependencies
# 
# 'make'		build executable file 'aes-encrypt'
# 'make clean'	removes all .o and executable files
#

# Defines the C Compiler used
CC=gcc

# Defines any compile-time flags
CFLAGS = -Wall -g

# Define the .o output directory
ODIR = bin

# Define the source directory
SDIR = src

# Defines the C source files
ESRCLIST = aes-encrypt.c base64.c bytesub.c keyexpand.c gf.c
DSRCLIST = aes-decrypt.c base64.c bytesub.c keyexpand.c gf.c

ESRCS = $(patsubst %,$(SDIR)/%,$(ESRCLIST))
DSRCS = $(patsubst %,$(SDIR)/%,$(DSRCLIST))

EOBJS = $(patsubst %,$(ODIR)/%,$(ESRCLIST:.c=.o))
DOBJS = $(patsubst %,$(ODIR)/%,$(DSRCLIST:.c=.o))

# Defines the executable files
AESE = aes-encrypt
AESD = aes-decrypt

.PHONY: depend clean 

all: $(AESE) $(AESD)
	@echo $(AESE), $(AESD) have been compiled

$(AESE): $(EOBJS)
	$(CC) $(CFLAGS) -o $(AESE) $(EOBJS)

$(AESD): $(DOBJS)
	$(CC) $(CFLAGS) -o $(AESD) $(DOBJS) 

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(ODIR)/*.o *~ $(MAIN)
