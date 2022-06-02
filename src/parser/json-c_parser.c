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
#include "json-c_parser.h"

#include "json-c_types.h"
#include "json-c_linked_list.h"
#include "json-c_utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Internal function forward declarations

static char *_jsonPolishValueString(const char* const str, char **out, const int offsetFromStart, const int numOfCharsToOmit);
static int _jsonParseValueString(const char** const str, JSONValue* const out);

// --------------------

int jsonParseFile(JSONParser *parser, const char *path)
{
    /* 
    1. Open the file
    2. Go through the file line by line
    3. Split each line into tokens:
        3.1: The first token is the key, the second token is the value string
        3.2: Parse the value string and turn it into a JSONValue struct
    4. Put the key and value into a JSONPair struct
    5. Add this new pair into a list of JSONPairs
    6. Convert the list of pairs into a regular C array
    7. Save this array into the parser for use by the user
    */

    // 1. Open the file
    FILE *file = fopen(path, "r");

    if(file == NULL)
        return -1;
    
    fseek(file, 0, SEEK_SET);
    
    JSONTree *parsedPairsTree = NULL;
    jsonTreeCreate(&parsedPairsTree);
    
    // 2. Go through the file line by line
    char *line = NULL;
    int lineLength = 0;
    char **splitLine = NULL;
    while(_jsonFileGetLine(&line, &lineLength, file) != 0)
    {
        // 3. Split each line into tokens
        int numOfStrings = 0;
        if(_jsonSplitString(&splitLine, line, ':', &numOfStrings) == 1)
        {
            // 3.1: The first token is the key, the second token is the value string
            const char* rawKeyStr = splitLine[0];
            const char* rawValueStr = splitLine[1];

            // Remove double quotes from the key string so that it doesn't goof up
            // JSONPair lookup later on
            // The -1 here gets rid of the first double quote
            // because it's going to be omitted in strncpy
            int keyStrLength = strlen(rawKeyStr) - 1;
            char* key = (char*)malloc(keyStrLength);
            strncpy(key, rawKeyStr + 1, keyStrLength);
            // strLength - 1 because arrays index from 0,
            // therefore a string of length 5 has the last index of 4
            key[keyStrLength - 1] = '\0';

            // 3.2: Parse the value string and turn it into a JSONValue struct  
            JSONValue jsonValueStruct;
            _jsonParseValueString(&rawValueStr, &jsonValueStruct);

            // 4. Put the key and value into a JSONPair struct
            // If a JSONObject or JSONArray are being parsed,
            // add the JSONValue struct to the pair once they are complete
            JSONPair *pair = (JSONPair*)malloc(sizeof(JSONPair));
            pair->key = key;
            pair->value = jsonValueStruct;
            
            // TODO: Add pair to the tree

            // Free the raw strings because they have been parsed and copied into new strings/values
            // Avoids a memory leak
            // Explicit void* cast here to get rid of warning that 'const' modifier is being discarded
            free((void*)rawKeyStr);
            free((void*)rawValueStr);
            // Free the current line because it's not needed anymore
            free(line);
            line = NULL;
        }
    }
    free(line);
    line = NULL;
    
    fclose(file);

    parser->pairs = parsedPairsTree;
    
    // TODO: Add checks whether everything has been parsed correctly. If not, return 0.
    return 1;
}

void jsonClearParser(JSONParser *parser)
{

}

// Utility function for removing X amount of characters from the start (+ specified offset) of the string
static char *_jsonPolishValueString(const char* const str, char** out, const int offsetFromStart, const int numOfCharsToOmit)
{
    int polishedStringLength = strlen(str) - numOfCharsToOmit;
    // Allocate +1 char for a null-termination character
    char* polishedString = (char*)malloc((polishedStringLength + 1) * sizeof(char));
    strncpy(polishedString, str + offsetFromStart, (size_t)(polishedStringLength));
    // Because of array indexes starting from 0,
    // a str of length 4 would have the indexes 0-3
    // and length leads directly to extra allocated char 
    // for null-termination char
    polishedString[polishedStringLength] = '\0';

    *out = polishedString;
    return polishedString;
}

static int _jsonParseValueString(const char** const str, JSONValue* const out)
{
    const char* const jsonString = *str;
    
    for (size_t i = 0; i < strlen(jsonString); i++)
    {
        // Judging by the first character in the string,
        // we can determine what type of value the pair holds
        // FIXME: Currently, if the first character is a whitespace, this whole code breaks
        //        Either pass in a string that's already free of them
        //        or figure out a way to ignore them
        if(i == 0)
        {
            switch(jsonString[i])
            {
                // If the first character is n(ull),
                // the value is guaranteed to be NULL
                // If the value cannot be identified, 
                // set the type to NULL as well
                case 'n':
                default:
                {
                    out->type = JSON_VALUE_TYPE_NULL;
                    out->value = NULL;
                    
                    return 1;
                }
                break;

                // If the first character is a digit,
                // the value is guaranteed to be a NUMBER
                // (either an int or a float)
                // NOTE: Using the isdigit() function would make more sense and would be easier,
                //       however, for the sake of cohesion, I decided to write it out here
                //       in the switch statement so all of the value type parsing stays grouped together
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    // The length of the string is the length
                    // -1 comma at the end of the line
                    char *valueString = NULL;
                    _jsonPolishValueString(jsonString, &valueString, 0, 1);

                    // If a dot is present in the string, the value is of type FLOAT,
                    // otherwise it's a regular INT
                    // NOTE: Adding support for the E float notation would be nice
                    //       (eg. 100.0 => 1E+2 or 1.0E+2)
                    if(strchr(valueString, '.') != NULL)
                    {
                        // Allocate on heap to ensure that the value
                        // doesn't delete after exiting this block
                        float *value = (float*)malloc(sizeof(float));
                        *value = atof(valueString);

                        out->type = JSON_VALUE_TYPE_FLOAT;
                        out->value = (void*)value;
                    }
                    else
                    {
                        int *value = (int*)malloc(sizeof(int));
                        *value = atoi(valueString);
                        
                        out->type = JSON_VALUE_TYPE_INT;
                        out->value = (void*)value;
                    }
                    
                    // Preserving the value string is not necessary
                    // because its use has reached its end
                    free(valueString);
                    return 1;
                }
                break;

                // If the first character is a double quote,
                // the type is guaranteed to be a STRING
                case '"':
                {
                    // The length of the string is the length
                    // -2 double quotes (start and end)
                    // -1 comma at the end of the line
                    char *valueString = NULL;
                    // Copy the string starting from past the start double quote
                    _jsonPolishValueString(jsonString, &valueString, 1, 3);

                    out->type = JSON_VALUE_TYPE_STRING;
                    out->value = (void*)valueString;

                    // Value successfully parsed
                    return 1;
                }
                break;

                // If the first character is t(rue) or f(alse),
                // the type is guaranteed to be a BOOL
                case 't':
                case 'f':
                {
                    // The length of the string is the length
                    // -1 comma at the end of the line
                    char *valueString = NULL;
                    _jsonPolishValueString(jsonString, &valueString, 0, 1);

                    // Because C doesn't have bools out of the box,
                    // use an int
                    int *value = (int*)malloc(sizeof(int));
                    *value = strcmp("true", valueString) == 0 ? 1 : 0;
                    
                    out->type = JSON_VALUE_TYPE_BOOL;
                    out->value = (void*)value;

                    return 1;
                }
                break;

                // TODO: Object parsing
                // TODO: Array parsing
            }
        }
    }
}