# You should always specify a range with the newest
# and oldest tested versions of CMake. 
cmake_minimum_required(VERSION 3.14...3.19)

# set version
set(version 1.0.0)

# This is your project statement. You should always list languages;
project(
    BoostBeastClient
    VERSION ${version}
    LANGUAGES CXX
)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED)

add_subdirectory(src)