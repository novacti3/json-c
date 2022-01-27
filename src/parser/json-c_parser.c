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

#include "json-c_utils.h"
#include <stdlib.h>
#include <string.h>

// Internal function forward declarations
static int _jsonParseValueString(const char** const str, JSONValue* const out);
// ----------

int jsonParseFile(JSONParser *parser, const char *path)
{
    /* 
    1. Open the file
    2. Go through the file line by line
    3. Split each line into tokens:
        3.1: The first token is the key, the second token is the value string
        3.2: Parse the value string and turn it into a JSONValue struct
    4. Put the key and value into a JSONPair struct
    5. Add this new pair into the parser's list of JSONPairs
    */

    // 1. Open the file
    FILE *file = fopen(path, "r");

    if(file == NULL)
        return -1;
    
    fseek(file, 0, SEEK_SET);
    
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
            // FIXME: Exclude the start and end double quotes in the key 
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
            // NOTE: This should probably be manually allocated 
            //       to ensure the lifetime
            JSONPair pair = 
            {
                .key = key, 
                .value = jsonValueStruct
            };

            // Free the raw strings because they have been parsed and copied into new strings
            // Avoids a memory leak
            free((void*)rawKeyStr);
            free((void*)rawValueStr);
        }
        // int keyStartQuoteIndex = -1;
        // int keyEndQuoteIndex = -1;
        // char *key = NULL;
        
        // int valueStartIndex = -1;
        // int valueEndIndex = -1;
        // JSONValue value;
        
        // JSONPair pair;

        // for (size_t i = 0; i < lineLength; i++)
        // {
        //     switch(line[i])
        //     {
        //         case '"':
        //         {
        //             if(key == NULL)
        //             {
        //                 if(keyStartQuoteIndex == -1) 
        //                     keyStartQuoteIndex = i;
                        
        //                 if((keyEndQuoteIndex == -1) && (i > keyStartQuoteIndex))
        //                     keyEndQuoteIndex = i;
        //             }
        //         }
        //         break;
            
        //         case ':':
        //         {
        //             size_t keySize = (keyEndQuoteIndex) - (keyStartQuoteIndex + 1);
        //             key = (char*)malloc(keySize);
        //             strncpy(key, &line[keyStartQuoteIndex + 1], keySize);
        //             key[keySize + 1] = '\0';
        //         }
        //         break;

        //         case ',':
        //         {
        //             if(i == lineLength - 1 && line[i + 1] == '\n')
        //             {

        //             }
        //         }
        //         break;

        //         case '{':
        //         {
        //             if(line[i] == 0)
        //                 continue;
        //         }
        //         break;
        //         case '}':
        //         {
        //             if(line[i] == lineLength)
        //                 continue;
        //         }
        //         break;
        //     }
        // }
    }

    fclose(file);
    // TODO: Add checks whether everything has been parsed correctly. If not, return 0.
    return 1;
}

void jsonClearParser(JSONParser *parser)
{

}

static int _jsonParseValueString(const char** const str, JSONValue* const out)
{
    const char* const jsonString = *str;
    
    for (size_t i = 0; i < strlen(jsonString); i++)
    {
        // Judging by the first character in the string,
        // we can determine what type of value we are talking about
        if(i == 0)
        {
            switch(jsonString[i])
            {
                // If the first character is a double quote,
                // the type is guaranteed to be a STRING
                case '"':
                {
                    // The length of the string is the length
                    // -1 double quote (only at the end because the first one is going to be excluded when copying the string)
                    // -1 comma at the end of the line
                    // -1 \n char at the very end of the line (TEMPORARY until I fix the _jsonGetLine func)
                    int valueStringLength = strlen(jsonString) - 1 - 1 - 1;
                    // Allocate +1 char for a null-termination character
                    char* valueString = (char*)malloc((valueStringLength + 1) * sizeof(char));

                    // Copy the string starting from past the start double quote
                    strncpy(valueString, jsonString + 1, (size_t)(valueStringLength));
                    // Because of array indexes starting from 0,
                    // a str of length 4 would have the indexes 0-3
                    // and length leads directly to extra allocated char 
                    // for null-termination char
                    valueString[valueStringLength] = '\0';

                    out->type = JSON_VALUE_TYPE_STRING;
                    out->value = (void*)valueString;

                    // Value successfully parsed
                    return 1;
                }
                break;
            }

        }
    }
}