cmake_minimum_required(VERSION 3.6)
project(untitled)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(PHP_INCLUDE_DIR "/usr/include/php/20131226") #PHP 5.6
set(PHP_INCLUDE_DIR "/usr/include/php/20151012") #PHP 7.0

include_directories(${PHP_INCLUDE_DIR}/main)
include_directories(${PHP_INCLUDE_DIR}/Zend)
include_directories(${PHP_INCLUDE_DIR}/TSRM)
include_directories(${PHP_INCLUDE_DIR})


set(SOURCE_FILES src/extension/php_levenshtein.cpp src/extension/levenshtein.cpp src/extension/storage.cpp src/extension/costs.cpp)
add_executable(php_levenshtein src/extension/php_levenshtein.cpp)
add_executable(levenshtein src/extension/levenshtein.cpp)
add_executable(costs src/extension/costs.cpp)