# LLVM-ROSE-Plugin

## Description

This simply links ROSE and Clang/LLVM together to call ROSE features within Clang.

This should behave like the ROSE identityTranslator.  Clang plugin calls ROSE frontend to process the inputfile with "-rose:skipfinalCompileStep" option. 

The SgProject object is stored as global variable.

A LLVM Pass calls ROSE backend afterward to generate unparsed code.  


## Building ROSE plugin  & ROSE LLVM Pass

- mkdir build; cd build
- cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DROSE_ROOT=${PATH_TO_ROSE_INSTALL_TREE} -DBoost_ROOT=${PATH_TO_BOOST_USED_BY_ROSE} ../
- make

libROSEPlugin.so and libROSEPass.so will be created in build directory.


## Running with ROSE plugin

- with clang driver: clang -Xclang -load -Xclang libROSEPlugin.so  test.c
- Using the cc1 command line: clang -cc1 -load libROSEPlugin.so -plugin ROSE test.c

## Running with ROSE LLVM Pass

- with clang river: clang -O1  -Xclang -load -Xclang libROSEPlugin.so  -fpass-plugin=libROSEPass.so test.c
- Using opt is not valid because this LLVM pass requires sgproject generated from the Clang plugin.

## Setup environment:

- ROSE with EDG frontend built by Clang compiler.
