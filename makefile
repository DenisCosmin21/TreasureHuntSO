CC = gcc
CFLAGS = -g -Wall -O3

# Executable names
TARGETS = treasure_manager treasure_hub monitor calculate_score

# Object files per target
TM_OBJS = TreasureManager.o Log.o Treasure.o DirectoryLib.o FileLib.o
TH_OBJS = TreasureHub.o operationHelpers.o FileLib.o DirectoryLib.o HubHelpers.o
M_OBJS  = treasureMonitor.o operationHelpers.o DirectoryLib.o FileLib.o MonitorHelpers.o
CS_OBJS = ScoreCalculator.o Treasure.o DirectoryLib.o FileLib.o Log.o AvlTree.o

all: $(TARGETS)

treasure_manager: $(TM_OBJS)
	$(CC) $(CFLAGS) $(STD) -o $@ $^

treasure_hub: $(TH_OBJS)
	$(CC) $(CFLAGS) $(STD) -o $@ $^

monitor: $(M_OBJS)
	$(CC) $(CFLAGS) $(STD) -o $@ $^

calculate_score: $(CS_OBJS)
	$(CC) $(CFLAGS) $(STD) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(STD) -c $< -o $@

clean:
	$(RM) *.o $(TARGETS)
