project(MHSJ)

cmake_minimum_required(VERSION 3.16)

#set(CMAKE_CXX_COMPILER /usr/bin/clang++)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -g -pedantic")

# add macros to generate ANTLR Cpp code from grammar
#find_package(BISON REQUIRED)
#find_package(FLEX REQUIRED)

#BISON_TARGET(MHSJParser ${CMAKE_CURRENT_SOURCE_DIR}/grammar/MHSJParser.yy ${CMAKE_CURRENT_BINARY_DIR}/MHSJParser.cpp
             #COMPILE_FLAGS "-Wall -Wcounterexamples --defines=${CMAKE_CURRENT_BINARY_DIR}/MHSJParser.h")
#FLEX_TARGET(MHSJScanner ${CMAKE_CURRENT_SOURCE_DIR}/grammar/MHSJScanner.ll  ${CMAKE_CURRENT_BINARY_DIR}/MHSJScanner.cpp
            #COMPILE_FLAGS "--debug --c++")
#ADD_FLEX_BISON_DEPENDENCY(MHSJScanner MHSJParser)

# include generated files in project environment
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/grammar)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/ErrorReport)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/ASTBuilder)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/ASTPrinter)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/StaticChecker)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/MHSJBuilder)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/mir)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Optimize)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/CodeGen)


# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/grammar)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ErrorReport)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/mir)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ASTBuilder)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ASTPrinter)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/StaticChecker)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/MHSJBuilder)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/Optimize)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/CodeGen)


add_executable(
  compiler
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)


target_link_libraries(
    compiler
    IRBuilder
    driver
    IR_lib
    err_report
    checker
    ASTPrinter
    pass
    CodeGen
)

install(
  TARGETS compiler
  RUNTIME DESTINATION bin
)
