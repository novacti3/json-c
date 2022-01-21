#include "json-c_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *getLine(int *length, FILE *stream)
{
    // The ptr to the start of the line in the stream
    char *startPtr = NULL;
    // The ptr to the end of the line in the stream
    char *endPtr = NULL;

    // Used to set the start pointer because when the stream is first passed in,
    // FILE->_ptr is set to NULL. Using fgetc sets the ptr to the appropriate place,
    // so setting startPtr is worth it only after fgetc is called for the first time.
    int firstIter = 1;
    while(feof(stream) == 0)
    {
        char c = fgetc(stream);
     
        if(firstIter)
        {
            // Function fgetc sets the file ptr to after the current character.
            // Seeing as we will need the entire line, we must compensate
            // by shifting the pointer one character back 
            // so we get the first character of the line as well. 
            startPtr = stream->_ptr - sizeof(char);
            firstIter = 0;
        }

        if(c == '\n')
        {
            // Same as startPtr, just with the last character of the line
            endPtr = stream->_ptr - sizeof(char);
            break;
        }
        else if(c == EOF)
        {
            return NULL;
        }
    }

    // Because we use pointers starting from the same memory address,
    // the length of the line is just the difference between the addresses
    // of the end and start pointers
    int lineLength = endPtr - startPtr;
    // Allocate enough space for all of the characters on the line + null-terminating character
    char *line = (char*)malloc((size_t)(lineLength + 1));
    
    // Shift the file pointer to the start of the line so that we can use
    // the fgets function and read lineLength characters to get the line string    
    int bytesToStartOfLine = (startPtr - stream->_base) * sizeof(char);
    fseek(stream, bytesToStartOfLine, SEEK_SET);
    fgets(line, lineLength + 1, stream);
    
    // Make sure to add a null-terminating character to the end of the line
    // so that it's a complete string
    line[lineLength + 1] = '\0';
    
    // Put the file ptr to the end of the line so that a new line can be read etc.
    // int bytesToEndOfLine = -bytesToStartOfLine;
    // fseek(stream, -bytesToEndOfLine, SEEK_CUR);
    // int bytesToEndOfLine = (endPtr - stream->_ptr) * sizeof(char);
    int result = fseek(stream, sizeof(char), SEEK_CUR);

    *length = lineLength;
    return line;
}

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
    while((line = getLine(&lineLength, file)) != NULL)
    {
        int keyStartQuoteIndex = -1;
        int keyEndQuoteIndex = -1;
        char *key;

        for (size_t i = 0; i < lineLength; i++)
        {
            switch(line[i])
            {
                case '"':
                {
                    keyStartQuoteIndex = keyStartQuoteIndex == -1 ? i : -1;
                    keyEndQuoteIndex = (keyEndQuoteIndex == -1) && (keyStartQuoteIndex != -1) ? i : -1; 
                }
                break;
            
                case ':':
                {
                    size_t keySize = (keyEndQuoteIndex - 1) - (keyStartQuoteIndex + 1);
                    key = (char*)malloc(keySize);
                    strncpy(key, &line[keyStartQuoteIndex], keySize);
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

    // char *line = NULL;
    // ssize_t lineLength = 0; 
    
    // while((lineLength = getline(&line, NULL, file)) != -1)
    // {
    //     int keyStartQuoteIndex = -1;
    //     int keyEndQuoteIndex = -1;
    //     char *key;

    //     for (size_t i = 0; i < lineLength; i++)
    //     {
    //         switch(line[i])
    //         {
    //             case '"':
    //             {
    //                 keyStartQuoteIndex = keyStartQuoteIndex == -1 ? i : -1;
    //                 keyEndQuoteIndex = (keyEndQuoteIndex == -1) && (keyStartQuoteIndex != -1) ? i : -1; 
    //             }
    //             break;
            
    //             case ':':
    //             {
    //                 size_t keySize = (keyEndQuoteIndex - 1) - (keyStartQuoteIndex + 1);
    //                 key = (char*)malloc(keySize);
    //                 strncpy(key, &line[keyStartQuoteIndex], keySize);
    //             }
    //             break;

    //             case '{':
    //             {
    //                 if(line[i] == 0)
    //                     continue;
    //             }
    //             break;
    //             case '}':
    //             {
    //                 if(line[i] == lineLength)
    //                     continue;
    //             }
    //             break;
    //         }
    //     }
    // }

    fclose(file);
    // TODO: Add checks whether everything has been parsed correctly. If not, return 0.
    return 1;
}

void jsonClearParser(JSONParser *parser)
{

}