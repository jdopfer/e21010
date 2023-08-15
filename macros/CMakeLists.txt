cmake_minimum_required(VERSION 3.18)
project(macros)

find_package(GSL REQUIRED)

add_executable(solid_angle solid_angle.cpp)
target_link_libraries(solid_angle PRIVATE AUSA::AUSA GSL::gsl)