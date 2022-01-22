# FIXME: Make the example build into a new directory in the directory along with the source code
# so that the example.json file doesn't have to be copied to who knows where
cmake_minimum_required(VERSION 3.20)

project(JSON-C_FILE_PARSING_EXAMPLE DESCRIPTION "Example of file parsing for the JSON-C library" LANGUAGES C)

add_executable(JSON-C_FILE_PARSING_EXAMPLE file_parsing.c)

set_target_properties(JSON-C_FILE_PARSING_EXAMPLE PROPERTIES LINKER_LANGUAGE C)
set_target_properties(JSON-C_FILE_PARSING_EXAMPLE PROPERTIES C_STANDARD 11)
set_target_properties(JSON-C_FILE_PARSING_EXAMPLE PROPERTIES C_STANDARD_REQUIRED 11)
set_target_properties(JSON-C_FILE_PARSING_EXAMPLE PROPERTIES OUTPUT_NAME "FileParsing")

target_link_libraries(JSON-C_FILE_PARSING_EXAMPLE PRIVATE JSON-C)
target_include_directories(JSON-C_FILE_PARSING_EXAMPLE PRIVATE "${JSON-C_SOURCE_DIR}/include")