cmake_minimum_required(VERSION 3.18)
project(sim)

find_package(G4Sim REQUIRED)

add_executable(sim main.cpp)
target_link_libraries(sim PRIVATE G4Sim::G4Sim)