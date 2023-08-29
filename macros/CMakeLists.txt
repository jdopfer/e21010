cmake_minimum_required(VERSION 3.18)
project(macros)

add_executable(solid_angle solid_angle.cpp)
target_link_libraries(solid_angle PRIVATE AUSA::AUSA GSL::gsl)

add_executable(pixel_matrices pixel_matrices.cpp)
target_link_libraries(pixel_matrices PRIVATE AUSA::AUSA GSL::gsl)