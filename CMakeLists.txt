cmake_minimum_required(VERSION 3.30)
project(TreasureHunt C)

set(CMAKE_C_STANDARD 99)

add_executable(TreasureHunt main.c
        Log.h
        Log.c
        Treasure.h
        Treasure.c
        DirectoryLib.h
        DirectoryLib.c
        FileLib.h
        FileLib.c)
