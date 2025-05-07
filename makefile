CC = gcc
CFLAGS = -g -Wall -O3

# Targets
TARGETS = treasure_manager.x treasure_hub.x

all: $(TARGETS)

# Object files for treasure_manager
TREASURE_MANAGER_OBJS = DirectoryLib.o FileLib.o Log.o Treasure.o TreasureManager.o

treasure_manager.x: $(TREASURE_MANAGER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Object files for treasure_hub
TREASURE_HUB_OBJS = DirectoryLib.o FileLib.o treasureMonitor.o operationHelpers.o TreasureHub.o

treasure_hub.x: $(TREASURE_HUB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	$(RM) *.o *.x