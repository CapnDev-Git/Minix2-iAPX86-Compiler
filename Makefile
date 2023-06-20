# Compiler
CC = gcc

# Compiler flags
CFLAGS =  -g -I./include #-Wall -Wextra -Werror -std=c99

# Directories
ODIR = obj
SRCDIR = src
INCDIR = include
TESTDIR = tests

# Libraries
LIBS =

# Dependencies
_DEPS = prints.h dump.h interpreter.h lexer.h parser.h globals.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ = main.o prints.o dump.o interpreter.o lexer.o parser.o globals.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TESTOBJ = utest.o prints.o
TESTOBJ = $(patsubst %,$(ODIR)/%,$(_TESTOBJ))

# Rules
$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(TESTDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Targets
all: interpreter $(TESTDIR)/utest

interpreter: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(TESTDIR)/utest: $(TESTOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(TESTDIR)/utest
	rm -f interpreter