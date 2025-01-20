export CC=gcc
export RM=rm -f
export AR=ar cr
export MV=mv
export RANLIB=ranlib
export SED=sed
export SHELL=/bin/sh

INCFLAGS=-I../common -I./include
CFLAGS=-m32 -w -O3 -g -pipe -Wall $(INCFLAGS) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

ifeq ($(shell uname),CYGWIN_NT-5.1)
	SQLSRC=sasql_cygwin.c
	SQL=-lodbc32 -lz -lpthread
	TARGET=./saacjt
endif
ifeq ($(shell uname),Linux)
	SQLSRC=sasql_linux.c
	SQL=-L /usr/lib/mysql/ -lmysqlclient -lz -lpthread
	TARGET=../../csa/saac/saacjt
endif

SRC=main.c char.c db.c mail.c acfamily.c lock.c $(SQLSRC)
OBJ=$(SRC:.c=.o)
COMMON_LIB=../common/libcommon.a
SERVER_LIB=../common/libsaacserver.a

${TARGET}: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(SERVER_LIB) $(COMMON_LIB) $(SQL)
	$(RM) $(OBJ)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

