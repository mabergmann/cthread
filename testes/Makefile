#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib

all: cyield semaforos cjoin cjoinErro

cjoinErro: cjoinErro.c $(LIB_DIR)/libcthread.a
	$(CC) -o cjoinErro cjoinErro.c -L$(LIB_DIR) -lcthread -Wall

cjoin: cjoin.c $(LIB_DIR)/libcthread.a
	$(CC) -o cjoin cjoin.c -L$(LIB_DIR) -lcthread -Wall

cyield: cyield.c $(LIB_DIR)/libcthread.a
	$(CC) -o cyield cyield.c -L$(LIB_DIR) -lcthread -Wall

semaforos: semaforos.c $(LIB_DIR)/libcthread.a
	$(CC) -o semaforos semaforos.c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf cyield semaforos cjoin cjoinErro *.o *~




