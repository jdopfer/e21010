cmake_minimum_required(VERSION 3.18)
project(eloss)

add_executable(Edep_vs_Ep Edep_vs_Ep.cpp include/common.h)
add_executable(telescope_deadareas telescope_deadareas.cpp include/common.h)
add_executable(thickness_from_puncthrough thickness_from_punchthrough.cpp include/common.h)
target_include_directories(Edep_vs_Ep PRIVATE include)
target_include_directories(telescope_deadareas PRIVATE include)
target_include_directories(thickness_from_puncthrough PRIVATE include)
