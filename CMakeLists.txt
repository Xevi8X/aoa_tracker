cmake_minimum_required(VERSION 3.15)
project(aoa_tracker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTLSDR REQUIRED librtlsdr)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(aoa_tracker main.cpp ${SOURCES})

target_include_directories(aoa_tracker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(aoa_tracker PRIVATE ${RTLSDR_INCLUDE_DIRS})
target_link_libraries(aoa_tracker PRIVATE ${RTLSDR_LIBRARIES})

add_executable(test tests/channel_determiner.cpp ${SOURCES})
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(test PRIVATE ${RTLSDR_INCLUDE_DIRS})
target_link_libraries(test PRIVATE ${RTLSDR_LIBRARIES})