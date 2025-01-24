INCFLAGS=-I../include -I../../lua -I../../common
CFLAGS=-m32 -w -O3 -Wall -pipe $(INCFLAGS)

TARGET=libbattle.a
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
MV=mv
RANLIB=ranlib
SED=sed
SHELL=/bin/sh
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(RM) $(TARGET)
	$(AR) $(TARGET) $(OBJ)
	$(RANLIB) $(TARGET)
	$(RM) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

