cmake_minimum_required(VERSION 3.18)
project(analysis)

link_libraries(AUSA::AUSA config++ telescope::telescope)

file(GLOB HEADERS CONFIGURE_DEPENDS "include/*.h")
add_executable(bp bp.cpp ${HEADERS})
target_include_directories(bp PRIVATE include)