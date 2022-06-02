APP1=creador
APP2=productor
APP3=consumidor
APP4=finalizador

CC=gcc
ROUTE=../bin/
FLAGS=-L../lib -I../include -lm -lmemoria -lsemaforo -lutil
FLAGS2=-L../lib -I../include -lm -lmemoria -lsemaforo -lutil

BIN1=$(ROUTE)$(APP1)
SRC1=$(APP1).c

BIN2=$(ROUTE)$(APP2)
SRC2=$(APP2).c

BIN3=$(ROUTE)$(APP3)
SRC3=$(APP3).c
BIN4=$(ROUTE)$(APP4)
SRC4=$(APP4).c


all: COMPILE1 COMPILE2 COMPILE3 COMPILE4 

COMPILE1:
		$(CC) -o $(BIN1) $(SRC1) $(FLAGS)

COMPILE2:
		$(CC) -o $(BIN2) $(SRC2) $(FLAGS)

COMPILE3:
		$(CC) -o $(BIN3) $(SRC3) $(FLAGS2)

COMPILE4:
		$(CC) -o $(BIN4) $(SRC4) $(FLAGS2)
