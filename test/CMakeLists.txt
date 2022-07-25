cmake_minimum_required(VERSION 3.5)

find_package(GTest REQUIRED)
message(STATUS "GTest found in ${GTEST_INCLUDE_DIRS}, ${GTEST_LIBRARIES}")

set(libs ${libname} ${GTEST_LIBRARIES})
set(CMAKE_CXX_FLAGS "-fno-strict-aliasing")

include_directories(${LIBSHMLITE_PROJECT_DIR}/include)

add_executable(test_shmhandle test_shmhandle.cc)
target_link_libraries(test_shmhandle ${libs})

add_executable(test_shmlock test_shmlock.cc)
target_link_libraries(test_shmlock ${libs})

add_executable(test_shmpool test_shmpool.cc)
target_link_libraries(test_shmpool ${libs})