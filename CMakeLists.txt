cmake_minimum_required(VERSION 3.30)
project(treasure_manager C)

set(CMAKE_C_STANDARD 99)

add_executable(treasure_manager TreasureManager.c
        Log.h
        Log.c
        Treasure.h
        Treasure.c
        DirectoryLib.h
        DirectoryLib.c
        FileLib.h
        FileLib.c
        treasureMonitor.h
)

add_executable(treasure_hub TreasureHub.c
        treasureMonitor.c
        operationHelpers.c
        operationHelpers.h
        FileLib.h
        FileLib.c
        DirectoryLib.h
        DirectoryLib.c
)
