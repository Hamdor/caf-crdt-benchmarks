cmake_minimum_required(VERSION 2.8)
project(caf-crdt-bench C CXX)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# silence policy CMP0042 warning by enabling RPATH explicitly
if(APPLE AND NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH true)
endif()

# prohibit in-source builds
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please use "
                        "./configure to choose a build directory and "
                        "initialize the build configuration.")
endif ()

# check if the user provided CXXFLAGS on the command line
if (CMAKE_CXX_FLAGS)
  set(CXXFLAGS_PROVIDED true)
else()
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wextra -Wall -pedantic -O3")
endif ()

# set build type (evaluate ENABLE_DEBUG flag)
if (ENABLE_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPPA_DEBUG")
else ()
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

# set(CAF_ROOT_DIR "/Users/noir/Git/actor-framework/build")
find_package(CAF COMPONENTS core io crdt)
if (CAF_FOUND)
  include_directories(${CAF_INCLUDE_DIRS})
else ()
  message(FATAL_ERROR "Could not find CAF")
endif ()

include_directories(. include)

file(GLOB_RECURSE HEADERS "include/*.hpp")

add_executable(caf-crdt-bench src/gset.cpp ${HEADERS})
target_link_libraries(caf-crdt-bench ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO} ${CAF_LIBRARY_CRDT})

#add_executable(caf-crdt-bench2 src/gset2.cpp ${HEADERS})
#target_link_libraries(caf-crdt-bench2 ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO} ${CAF_LIBRARY_CRDT})

add_executable(caf-crdt-convergence-dist src/convergence_distributed.cpp ${HEADERS})
target_link_libraries(caf-crdt-convergence-dist ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO} ${CAF_LIBRARY_CRDT})

add_executable(caf-crdt-convergence src/convergence.cpp ${HEADERS})
target_link_libraries(caf-crdt-convergence ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO} ${CAF_LIBRARY_CRDT})

add_executable(caf-crdt-size src/size_bench.cpp ${HEADERS})
target_link_libraries(caf-crdt-size ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO} ${CAF_LIBRARY_CRDT})

add_executable(caf-crdt-size-all src/all_write_bench.cpp ${HEADERS})
target_link_libraries(caf-crdt-size-all ${CMAKE_DL_LIBS} ${CAF_LIBRARY_CORE} ${CAF_LIBRARY_IO} ${CAF_LIBRARY_CRDT})
