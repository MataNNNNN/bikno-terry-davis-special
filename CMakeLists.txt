cmake_minimum_required(VERSION 3.10)

project(Bikno)

set(CMAKE_CXX_STANDARD 20)

add_executable(Bikno
    main.cpp
    Lexer/lexer.cpp
    Parser/parser.cpp
    Generation/values.cpp
    Generation/operators.cpp
    Generation/generation.cpp
)

target_include_directories(Bikno PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser
    ${CMAKE_CURRENT_SOURCE_DIR}/Generation
)