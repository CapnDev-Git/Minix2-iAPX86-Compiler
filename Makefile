# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -I$(INCDIR) #-Wall -Wextra -Werror -std=c99

# Directories
ODIR = obj
SRCDIR = src
INCDIR = include
TESTDIR = tests

# Libraries
LIBS =

# Source files
SRCS := $(wildcard $(SRCDIR)/**/*.c $(SRCDIR)/*.c)
SRCDIRS := $(sort $(dir $(SRCS)))
OBJS := $(patsubst $(SRCDIR)/%.c,$(ODIR)/%.o,$(SRCS))

# Test source files
TESTSRCS := $(wildcard $(TESTDIR)/*.c)
TESTOBJS := $(patsubst $(TESTDIR)/%.c, $(ODIR)/%.o, $(TESTSRCS))

# Object directory creation
$(shell mkdir -p $(ODIR) $(SRCDIRS:$(SRCDIR)%=$(ODIR)%))

# Rules
$(ODIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(TESTDIR)/%.c $(INCDIR)/prints.h $(INCDIR)/globals.h $(TESTDIR)/utest.h
	$(CC) -c -o $@ $< $(CFLAGS)

# Targets
all: z123029_mmvm $(TESTDIR)/utest

z123029_mmvm: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(TESTDIR)/utest: $(TESTOBJS) $(ODIR)/global/globals.o $(ODIR)/global/prints.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -rf $(ODIR)/*
	rm -f z123029_mmvm
	rm -f $(TESTDIR)/utest