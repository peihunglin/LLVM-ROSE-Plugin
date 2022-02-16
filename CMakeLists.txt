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
               # ClangRosePlugin.export contains C++ symbols.
  if( NOT LLVM_REQUIRES_RTTI )
    if( NOT LLVM_REQUIRES_EH )
      set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ClangRosePlugin.exports)
    endif()
  endif()
endif()

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS} ${ROSE_ROOT}/include/rose ${Boost_INCLUDE_DIRS})
add_library(ClangRosePlugin MODULE ClangRosePlugin.cpp)
if (APPLE)
   target_link_libraries(ClangRosePlugin clangAST clangBasic clangFrontend rose -L${ROSE_ROOT}/lib)
else ()
   target_link_libraries(ClangRosePlugin  rose -L${ROSE_ROOT}/lib)
endif ()
#target_link_libraries(ClangRosePlugin clangAST clangBasic clangFrontend)

if(LLVM_ENABLE_PLUGINS AND (WIN32 OR CYGWIN))
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(ClangRosePlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()


add_library(LLVMRosePass MODULE LLVMRosePass.cpp)
target_compile_features(LLVMRosePass PRIVATE cxx_range_for cxx_auto_type)
target_link_libraries(LLVMRosePass rose -L${ROSE_ROOT}/lib)

if(APPLE)
    set_target_properties(LLVMRosePass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)


