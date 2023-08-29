cmake_minimum_required(VERSION 3.18)
project(e21010)

find_package(GSL REQUIRED)
find_package(ROOT REQUIRED)
find_package(AUSALIB REQUIRED) # https://gitlab.au.dk/ausa/ausalib
find_package(telescope REQUIRED) # https://gitlab.au.dk/ausa/erik/telescope
find_package(libconfig++ REQUIRED) # https://github.com/hyperrealm/libconfig -- available in most UNIX package managers!

include(FetchContent) # Make `FetchContent_Declare` and `FetchContent_MakeAvailable` available
FetchContent_Declare(eutil GIT_REPOSITORY https://gitlab.au.dk/ausa/erik/eutil.git)
FetchContent_MakeAvailable(eutil) # EUtil is a tiny project which provides easy C++ path handling
FetchContent_Populate(linlibertine # Linux Libertine Fonts
    URL https://downloads.sourceforge.net/project/linuxlibertine/linuxlibertine/5.3.0/LinLibertineOTF_5.3.0_2012_07_02.tgz
    URL_HASH MD5=e859a19401464097ab35226fdea64c3e
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/fonts)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/.projectroot)
  file(WRITE ${CMAKE_SOURCE_DIR}/.projectroot ${CMAKE_SOURCE_DIR})
endif()



include_directories(include)
# Parse CMakeLists.txt files in subdirectories
add_subdirectory(analysis)
#add_subdirectory(beamspot)
add_subdirectory(macros)
add_subdirectory(sim)