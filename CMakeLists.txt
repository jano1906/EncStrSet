cmake_minimum_required(VERSION 3.17)
project(JNPzad2)

set(CMAKE_CXX_STANDARD 17)


set(SOURCES main.cpp encstrset.cc encstrset.h)
set(HEADER_FILES encstrset.h)


add_executable(e ${SOURCES})