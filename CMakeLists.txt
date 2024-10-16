cmake_minimum_required(VERSION 3.20)

project(real_number)

add_subdirectory(rational_number)

add_library(real_number
    real_number.hpp
    real_number.cpp
)
target_include_directories(real_number PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(real_number PUBLIC cxx_std_23)
target_link_libraries(real_number PUBLIC rational_number)

add_executable(real_number_test
    real_number_test.cpp
)
target_link_libraries(real_number_test PUBLIC real_number)