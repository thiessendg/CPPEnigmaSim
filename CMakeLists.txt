cmake_minimum_required(VERSION 3.10)
project(CppEnigmaSim)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(CppEnigmaSim
        AddSubMod.h
        constants.h
        Enigma.h
        main.cpp
        plugboard.h
        reflector.h
        rotor.h
        wheel.h)
