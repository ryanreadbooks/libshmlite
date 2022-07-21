cmake_minimum_required(VERSION 3.5)

find_package(GTest REQUIRED)
message(STATUS "GTest found in ${GTEST_INCLUDE_DIRS}, ${GTEST_LIBRARIES}")

set(libs ${libname} ${GTEST_LIBRARIES})

include_directories(${LIBSHMLITE_PROJECT_DIR}/include)

add_executable(test_shmhandle test_shmhandle.cc)
target_link_libraries(test_shmhandle ${libs})