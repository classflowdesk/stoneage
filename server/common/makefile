export CC=gcc
export RM=rm -f
export AR=ar cr
export RANLIB=ranlib
export SHELL=/bin/sh

INCFLAGS=-I.
CFLAGS=-m32 -w -O3 -Wall -pipe $(INCFLAGS)
SAAC_CFLAGS=$(CFLAGS) -I../saac/include
GMSV_CFLAGS=$(CFLAGS) -I../gmsv/include -I../gmsv/src/lua

TARGET=libcommon.a libsaacserver.a libsaacclient.a libgmsvserver.a
LIBCOMMON=libcommon.a
LIBSAACSERVER=libsaacserver.a
LIBSAACCLIENT=libsaacclient.a
LIBGMSVSERVER=libgmsvserver.a

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
COMMON_SRC=md5.c util.c server_util.c workspace.c
COMMON_OBJ=$(COMMON_SRC:.c=.o)

SAACSERVER_SRC=saac_server_send.c saac_server_recv.c
SAACSERVER_OBJ=$(SAACSERVER_SRC:.c=.o)
SAACCLIENT_SRC=saac_client_send.c saac_client_recv.c
SAACCLIENT_OBJ=$(SAACCLIENT_SRC:.c=.o)
GMSVSERVER_SRC=gmsv_server_send.c gmsv_server_recv.c
GMSVSERVER_OBJ=$(GMSVSERVER_SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
RANLIB=ranlib
SHELL=/bin/sh
endif

all: copyright $(LIBCOMMON) $(LIBSAACSERVER) $(LIBSAACCLIENT) $(LIBGMSVSERVER)

${LIBCOMMON}: $(COMMON_OBJ)
	$(RM) $(LIBCOMMON)
	$(AR) $(LIBCOMMON) $(COMMON_OBJ)
	$(RANLIB) $(LIBCOMMON)
	$(RM) $(COMMON_OBJ)

$(COMMON_OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

${LIBSAACSERVER}: $(SAACSERVER_OBJ)
	$(RM) $(LIBSAACSERVER)
	$(AR) $(LIBSAACSERVER) $(SAACSERVER_OBJ)
	$(RANLIB) $(LIBSAACSERVER)
	$(RM) $(SAACSERVER_OBJ)

$(SAACSERVER_OBJ): %.o: %.c
	$(CC) $(SAAC_CFLAGS) -c $< -o $@

${LIBSAACCLIENT}: $(SAACCLIENT_OBJ)
	$(RM) $(LIBSAACCLIENT)
	$(AR) $(LIBSAACCLIENT) $(SAACCLIENT_OBJ)
	$(RANLIB) $(LIBSAACCLIENT)
	$(RM) $(SAACCLIENT_OBJ)

$(SAACCLIENT_OBJ): %.o: %.c
	$(CC) $(GMSV_CFLAGS) -c $< -o $@

${LIBGMSVSERVER}: $(GMSVSERVER_OBJ)
	$(RM) $(LIBGMSVSERVER)
	$(AR) $(LIBGMSVSERVER) $(GMSVSERVER_OBJ)
	$(RANLIB) $(LIBGMSVSERVER)
	$(RM) $(GMSVSERVER_OBJ)

$(GMSVSERVER_OBJ): %.o: %.c
	$(CC) $(GMSV_CFLAGS) -c $< -o $@

copyright:
	$(SHELL) ./copyright.sh

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ)
	$(RM) *~

print:
	echo $(COMMON_SRC)
	echo $(COMMON_OBJ)
