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

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
file(COPY "example.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/bin")

project(JSON-C_EXAMPLE_FILE_PARSING DESCRIPTION "Example of file parsing for the JSON-C library" LANGUAGES C)

add_executable(JSON-C_EXAMPLE_FILE_PARSING file_parsing.c)

set_target_properties(JSON-C_EXAMPLE_FILE_PARSING PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED 11
    LINKER_LANGUAGE C

    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    OUTPUT_NAME "FileParsing"
)

target_link_libraries(JSON-C_EXAMPLE_FILE_PARSING PRIVATE JSON-C)
target_include_directories(JSON-C_EXAMPLE_FILE_PARSING PRIVATE "${JSON-C_SOURCE_DIR}/include")