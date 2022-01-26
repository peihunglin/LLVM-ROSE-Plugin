# LLVM-ROSE-Plugin

## Description

This simply links ROSE and Clang/LLVM together to call ROSE features within Clang.

This should behave like the ROSE identityTranslator.  ROSE frontend is called to process the inputfile with "-rose:skipfinalCompileStep" option.  ROSE backend is called afterward to generate unparsed code.  


## Building ROSE plugin

- mkdir build; cd build
- cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DROSE_ROOT=${PATH_TO_ROSE_INSTALL_TREE} -DBoost_ROOT=${PATH_TO_BOOST_USED_BY_ROSE} ../
- make

libROSEPlugin.so will be created in build directory.


## Running with ROSE plugin

- with clang driver: clang -Xclang -load -Xclang libROSEPlugin.so  test.c
- Using the cc1 command line: clang -cc1 -load libROSEPlugin.so -plugin ROSE test.c

## Setup environment:

- ROSE with EDG frontend built by Clang compiler.
