project(ROSEPLUGIN)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules"  ${CMAKE_MODULE_PATH})
include(FindROSE)
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED)
find_package(ROSE REQUIRED)
find_package(Boost REQUIRED)
set(CMAKE_CXX_VISIBILITY_PRESET default)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 0)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_CMAKE_DIR}"
  )

# import LLVM CMake functions
include(AddLLVM)

if( NOT MSVC ) # MSVC mangles symbols differently, and
               # ROSEPlugin.export contains C++ symbols.
  if( NOT LLVM_REQUIRES_RTTI )
    if( NOT LLVM_REQUIRES_EH )
      set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ROSEPlugin.exports)
    endif()
  endif()
endif()

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS} ${ROSE_ROOT}/include/rose ${Boost_INCLUDE_DIRS})
add_library(ROSEPlugin MODULE ROSEPlugin.cpp)
target_link_libraries(ROSEPlugin clangAST clangBasic clangFrontend rose -L${ROSE_ROOT}/lib)
#target_link_libraries(ROSEPlugin clangAST clangBasic clangFrontend)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(ROSEPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()


add_library(ROSEPass MODULE ROSEPass.cpp)
target_compile_features(ROSEPass PRIVATE cxx_range_for cxx_auto_type)
target_link_libraries(ROSEPass rose -L${ROSE_ROOT}/lib)

if(APPLE)
    set_target_properties(ROSEPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)


