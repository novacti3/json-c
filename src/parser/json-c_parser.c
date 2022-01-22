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

int jsonParseFile(JSONParser *parser, const char *path)
{
    /* 
    Open the file
    Go through the file line by line
    
    Split each line into tokens:
        Ignore the first and last line (they are just {} signifying the root JSON object)
        Go through the line character by character
        
        First parse the key. Save the first " char index, 
        then keep going until the second " is encountered.
        Our key is (endQuoteIndex - 1) - (startQuoteIndex + 1). 
        The -1 and +1 gets rid of the actual " char and only extracts the actual key string.
        Store in a key string.

        Check for :. After encountering it, start checking for the value
        Value parsing WIP

        Save in a JSONPair object.

        If the last character of the line is ',', start a new JSONPair and repeat.
    */
    FILE *file = fopen(path, "r");

    if(file == NULL)
        return -1;
    
    fseek(file, 0, SEEK_SET);
    
    char *line = NULL;
    int lineLength = 0;
    while(jsonFileGetLine(&line, &lineLength, file) != 0)
    {
        int keyStartQuoteIndex = -1;
        int keyEndQuoteIndex = -1;
        char *key = NULL;
        
        int valueStartIndex = -1;
        int valueEndIndex = -1;
        JSONValue value;
        
        JSONPair pair;

        for (size_t i = 0; i < lineLength; i++)
        {
            switch(line[i])
            {
                case '"':
                {
                    if(key == NULL)
                    {
                        if(keyStartQuoteIndex == -1) 
                            keyStartQuoteIndex = i;
                        
                        if((keyEndQuoteIndex == -1) && (i > keyStartQuoteIndex))
                            keyEndQuoteIndex = i;
                    }
                }
                break;
            
                case ':':
                {
                    size_t keySize = (keyEndQuoteIndex) - (keyStartQuoteIndex + 1);
                    key = (char*)malloc(keySize);
                    strncpy(key, &line[keyStartQuoteIndex + 1], keySize);
                    key[keySize + 1] = '\0';
                }
                break;

                case ',':
                {
                    if(i == lineLength - 1 && line[i + 1] == '\n')
                    {

                    }
                }
                break;

                case '{':
                {
                    if(line[i] == 0)
                        continue;
                }
                break;
                case '}':
                {
                    if(line[i] == lineLength)
                        continue;
                }
                break;
            }
        }
    }

    fclose(file);
    // TODO: Add checks whether everything has been parsed correctly. If not, return 0.
    return 1;
}

void jsonClearParser(JSONParser *parser)
{

}