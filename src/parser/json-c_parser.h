/* Copyright © 2022 Jan Šaler

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the “Software”), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
IN THE SOFTWARE.
*/

/*
JSON file format:
- written in "key":value pairs
- values can have the following types:
    - number
    - string
    - bool
    - array -- list of values (eg. ["Apple", "Pear", "Peach"...] )
    - object -- collection of key/value pairs (eg. { "name":"Jack", "age":5, ...} )
- Objects can be nested together to represent the structure of data
*/
#ifndef JSON_C_PARSER
#define JSON_C_PARSER

#include "json-c_types.h"

// Parser object that reads a JSON file at the provided filepath holds the extracted data
typedef struct JSONParser
{
    char *filePath;
    JSONPair *pairs;
} JSONParser;

// Parses the provided file and stores all of the key/value pairs contained within
// Returns 1 if parsing was successful, 0 if parsing failed and -1 if file couldn't be found
int jsonParseFile(JSONParser *parser, const char *path);
// Clears the JSONParser of all data and prepares it for re-use by jsonParseFile()
void jsonClearParser(JSONParser *parser);

#endif