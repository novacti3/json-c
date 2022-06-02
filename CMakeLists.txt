# Copyright © 2022 Jan Šaler

# Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
# and associated documentation files (the “Software”), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute, 
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished
# to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all copies 
# or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
# AND NONINFRINGEMENT. 
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
# IN THE SOFTWARE.
cmake_minimum_required(VERSION 3.20)

file(MAKE_DIRECTORY build/lib)

project(JSON-C DESCRIPTION "JSON parse/serialization library written in C11" LANGUAGES C)

option(JSON-C_COMPILE_SERIALIZATION "Compiles the serialization part of the library" ON)
option(JSON-C_BUILD_SHARED "Builds a shared library instead of a static one" OFF)
option(JSON-C_BUILD_TESTS "Builds the library's unit tests" OFF)
option(JSON-C_BUILD_EXAMPLES "Builds the examples alongside the library" OFF)

list(APPEND CORE_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core"
)
list(APPEND CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/json-c_utils.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/json-c_linked_list.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/json-c_tree.c"
)


# Only the parser is compiled by default, as it is the essential part of the library
list(APPEND PARSER_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser"
)
list(APPEND PARSER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/json-c_parser.c"
)
# If serialization is not to be compiled, simply leave their includes and sources empty
list(APPEND SERIALIZER_INCLUDES)
list(APPEND SERIALIZER_SOURCES)
list(APPEND DESERIALIZER_INCLUDES)
list(APPEND DESERIALIZER_SOURCES)

# Else fill them up with the required filepaths so that they can be added to the lib later
if(JSON-C_COMPILE_SERIALIZATION)
    list(APPEND SERIALIZER_INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/serializer"
    )
    list(APPEND SERIALIZER_SOURCES
    
    )

    list(APPEND DESERIALIZER_INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/deserializer"
    )
    list(APPEND DESERIALIZER_SOURCES
    
    )
endif()

if(JSON-C_BUILD_SHARED)
    add_library(JSON-C SHARED)
else()
    add_library(JSON-C STATIC)
endif()

set_target_properties(JSON-C PROPERTIES 
    C_STANDARD 11
    C_STANDARD_REQUIRED 11
    LINKER_LANGUAGE C

    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    OUTPUT_NAME "json-c"
)

add_compile_definitions(JSON_C_SERIALIZATION_COMPILED=1)
# FIXME: As it stands right now, the user can include EVERY possible include. This clutters up IntelliSense and leads to confusion
#        Figure out a way to let the user include only json-c.h
target_include_directories(JSON-C PUBLIC ${CORE_INCLUDES} ${PARSER_INCLUDES} ${SERIALIZER_INCLUDES} ${DESERIALIZER_INCLUDES})
target_sources(JSON-C 
    PRIVATE ${CORE_SOURCES} 
    PUBLIC ${PARSER_SOURCES} ${SERIALIZER_SOURCES} ${DESERIALIZER_SOURCES}
)

if(JSON-C_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(JSON-C_BUILD_EXAMPLES)
    add_subdirectory(examples/file_parsing)
endif()