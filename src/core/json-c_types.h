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
#ifndef JSON_C_TYPES
#define JSON_C_TYPES

// An enum containing all of the possible types a value can be
typedef enum JSONValueType
{
    JSON_VALUE_TYPE_NULL,
    JSON_VALUE_TYPE_INT,
    JSON_VALUE_TYPE_FLOAT,
    JSON_VALUE_TYPE_STRING,
    JSON_VALUE_TYPE_BOOL,
    JSON_VALUE_TYPE_ARRAY,
    JSON_VALUE_TYPE_OBJECT
} JSONValueType;
// Holds the value of a key/value pair and its type
typedef struct JSONValue
{
    void *value;
    JSONValueType type;
} JSONValue;

// Struct that holds a single key/value pair
typedef struct JSONPair
{
    char *key;
    JSONValue value;
} JSONPair;

#endif