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
        HubHelpers.c
)

add_executable(treasure_hub TreasureHub.c
        operationHelpers.c
        operationHelpers.h
        FileLib.h
        FileLib.c
        DirectoryLib.h
        DirectoryLib.c
        HubHelpers.h
        HubHelpers.c
)

add_executable(monitor treasureMonitor.c
        operationHelpers.c
        operationHelpers.h
        DirectoryLib.c
        DirectoryLib.h
        FileLib.c
        FileLib.h
        MonitorHelpers.h
        MonitorHelpers.c
)

add_executable(calculate_score ScoreCalculator.c
        ScoreCalculator.c
        Treasure.c
        Treasure.h
        DirectoryLib.h
        DirectoryLib.c
        FileLib.h
        FileLib.c
        Log.c
        Log.h
)
