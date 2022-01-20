cmake_minimum_required(VERSION 3.20)

file(MAKE_DIRECTORY bin)

project(JSON-C DESCRIPTION "JSON parse/serialization library written in C11" LANGUAGES C)

# Controls whether the serialization part of the library 
# will be compiled and included in the final build
# If set to OFF, serialization functions will be excluded
# from the json-c.h header
set(JSON-C_COMPILE_SERIALIZATION ON)
# Builds a shared library instead of a static one
set(JSON-C_SHARED_LIB OFF)

# Only the parser is compiled by default, as it is the essential part of the library
list(APPEND PARSER_INCLUDES
    "src/parser/json-c_parser.h"
)
list(APPEND PARSER_SOURCES

)
# If serialization is not to be compiled, simply leave their includes and sources empty
list(APPEND SERIALIZER_INCLUDES)
list(APPEND SERIALIZER_SOURCES)
list(APPEND DESERIALIZER_INCLUDES)
list(APPEND DESERIALIZER_SOURCES)

# Else fill them up with the required filepaths so that they can be added to the lib later
if(JSON-C_COMPILE_SERIALIZATION)
    list(APPEND SERIALIZER_INCLUDES
        "src/serializer/json-c_serializer.h"
    )
    list(APPEND SERIALIZER_SOURCES
    
    )

    list(APPEND DESERIALIZER_INCLUDES
        "src/deserializer/json-c_deserializer.h"
    )
    list(APPEND DESERIALIZER_SOURCES
    
    )
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin/$<CONFIG>)

if(JSON-C_SHARED_LIB)
    add_library(JSON-C SHARED)
else()
    add_library(JSON-C STATIC)
endif()

set_target_properties(JSON-C PROPERTIES C_STANDARD 11)
set_target_properties(JSON-C PROPERTIES C_STANDARD_REQUIRED 11)
set_target_properties(JSON-C PROPERTIES LINKER_LANGUAGE C)

add_compile_definitions(JSON-C_SERIALIZATION_COMPILED=1)
target_include_directories(JSON-C PRIVATE ${PARSER_INCLUDES} ${SERIALIZER_INCLUDES} ${DESERIALIZER_INCLUDES})
target_sources(JSON-C PRIVATE ${PARSER_INCLUDES} ${SERIALIZER_INCLUDES} ${DESERIALIZER_INCLUDES})