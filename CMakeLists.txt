cmake_minimum_required(VERSION 3.0)

project(lxnwow CXX)
#set_property(GLOBAL PROPERTY CXX_STANDARD 17)
SET(CMAKE_CXX_COMPILER /usr/bin/g++-8)
#set(CMAKE_CXX_STANDARD 20)
#set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -fpermissive -m32 -std=c++2a")
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)

include_directories(
    src/
    src/third_party/
    src/third_party/AmeisenNavigation/recastnavigation/Detour/Include
    src/third_party/AmeisenNavigation/recastnavigation/Recast/Include
    src/third_party/AmeisenNavigation/recastnavigation/DetourTileCache/Include
   
)

file(GLOB THIRD_PARTY

    src/third_party/imgui/*.cpp
    src/third_party/imgui/*.h

    src/third_party/simple-matrix/src/*.cpp
    src/third_party/simple-matrix/src/*.hpp

    src/third_party/subhook/subhook/*.c
    src/third_party/subhook/subhook/*.h

    src/third_party/AmeisenNavigation/AmeisenNavigation/*.h
    src/third_party/AmeisenNavigation/AmeisenNavigation/*.cpp
    src/third_party/AmeisenNavigation/recastnavigation/Detour/Source/*.cpp
    src/third_party/AmeisenNavigation/recastnavigation/Recast/Source/*.cpp
    src/third_party/AmeisenNavigation/recastnavigation/DetourTileCache/Source/*.cpp
    


)

file(GLOB SOURCES 
    src/main.cpp
    src/ui/*.hpp
    src/ui/*.cpp
    src/hooks/*.hpp
    src/hooks/*.cpp
    src/wow/*.hpp
    src/wow/*.cpp
)

#path to lib directory
LINK_DIRECTORIES(
    "py"
    "libs"
    #"libs/mysql-connector-c-6.1.11-linux-glibc2.12-i686/lib"
)



add_library(lxnwow SHARED ${SOURCES} ${THIRD_PARTY})
set_target_properties(lxnwow PROPERTIES LINKER_LANGUAGE CXX)

# link libs from
target_link_libraries(lxnwow 
    pthread 
    stdc++fs 
    stdc++ 
    python3.6m 
    mysqlclient 
    #mysqlcppcon
)
