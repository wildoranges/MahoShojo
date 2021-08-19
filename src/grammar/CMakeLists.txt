add_library(
    driver
    MHSJDriver.cpp   
)

add_library(
    scanner STATIC
    MHSJScanner.cpp
)

add_library(
    parser STATIC
    MHSJParser.cpp   
)

target_link_libraries(
    driver
    parser
    scanner
    AST_lib
)