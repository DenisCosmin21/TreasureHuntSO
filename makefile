CC = gcc

CFLAGS = -g -Wall -O3

TARGET = treasure_manager

all : ${TARGET}

$(TARGET):
	$(CC) $(CFLAGS) -c DirectoryLib.c
	$(CC) $(CFLAGS) -c FileLib.c
	$(CC) $(CFLAGS) -c Log.c
	$(CC) $(CFLAGS) -c Treasure.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -o $(TARGET).x DirectoryLib.o FileLib.o Log.o Treasure.o main.o

clean:
	$(RM) $(TARGET).x
	$(RM) DirectoryLib.o
	$(RM) FileLib.o
	$(RM) Log.o
	$(RM) Treasure.o
	$(RM) main.o