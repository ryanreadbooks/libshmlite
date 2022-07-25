cmake_minimum_required(VERSION 3.5)
set(projectname libshmlite)
set(libname shmlite)
set(libversion 1.0.0)
set(libsoversion 1)
project(${projectname})

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No CMAKE_BUILD_TYPE OR CMAKE_CONFIGURATION_TYPES was specified, use Debug build type")
  set(CMAKE_BUILD_TYPE "Debug")
endif ()

option(BUILD_TESTS "Build all test cases" ON)
message(STATUS "BUILD_TESTS options: " ${BUILD_TESTS})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "add_definitions -DDEV_DEBUG")
  add_definitions(-DDEV_DEBUG)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -ggdb -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall -Werror")

set(libshmlite_inc
    include/libshmlite/common_utils.h
    include/libshmlite/shm_handle.h
    include/libshmlite/shm_lock.h
    include/libshmlite/shm_pool.hpp
    )

set(libshmlite_src
    src/libshmlite/common_utils.cc
    src/libshmlite/shm_handle.cc
    src/libshmlite/shm_lock.cc
    )

# 指定需要依赖的外部库
set(ext_libs pthread rt)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(LIBSHMLITE_PROJECT_DIR ${PROJECT_SOURCE_DIR})

add_library(${libname} SHARED ${libshmlite_src})

target_include_directories(${libname} PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(${libname} PROPERTIES VERSION ${libversion} SOVERSION ${libsoversion} SONAME ${libname})

target_link_libraries(${libname} PUBLIC ${ext_libs})

if (BUILD_TESTS)
  add_subdirectory(test)
endif ()

# 头文件默认安装到/usr/local/include
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION "include")
# 库文件默认安装到/usr/local/lib，指定0755权限
install(TARGETS ${libname}
    LIBRARY
    DESTINATION "lib"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)