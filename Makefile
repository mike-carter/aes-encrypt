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
SRCLIST = aes-encrypt.c

SRCS = $(patsubst %,$(SDIR)/%,$(SRCLIST))
OBJS = $(patsubst %,$(ODIR)/%,$(SRCLIST:.c=.o))

# Defines the executable file
MAIN = aes-encrypt

.PHONY: depend clean 

all: $(MAIN)
	@echo $(MAIN) has been compiled

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) 

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(ODIR)/*.o *~ $(MAIN)
