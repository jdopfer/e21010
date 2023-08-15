cmake_minimum_required(VERSION 3.18)
project(analysis)

link_libraries(AUSA::AUSA config++ telescope::telescope)

file(GLOB HEADERS CONFIGURE_DEPENDS "bp/include/*.h")
add_executable(bp bp/bp.cpp ${HEADERS})
target_include_directories(bp PRIVATE bp/include)

add_executable(solid_angle_matrix solid_angle_matrix/solid_angle_matrix.cpp)

add_subdirectory(eloss)