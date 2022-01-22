#include "json-c_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int jsonFileGetLine(char **out, int *length, FILE *stream)
{
    free(out);

    // The ptr to the start of the line in the stream
    int startCharIndex = -1;
    // The ptr to the end of the line in the stream
    int endCharIndex = -1;

    // Used to set the start pointer because when the stream is first passed in,
    // the file ptr is set to NULL. Using fgetc sets the ptr to the appropriate place,
    // so setting startCharIndex is worth it only after fgetc is called for the first time.
    int firstIter = 1;
    while(feof(stream) == 0)
    {
        char c = fgetc(stream);
     
        if(firstIter)
        {
            startCharIndex = ftell(stream) - 1;
            firstIter = 0;
        }

        if(c == '\n')
        {
            // Omit the \n at the end of the line because it's
            // redundant in the output string
            endCharIndex = ftell(stream) - 1;
            break;
        }
        else if(c == EOF)
        {
            return 0;
        }
    }

    int lineLength = endCharIndex - startCharIndex;
    // Allocate enough space for all of the characters on the line
    *out = (char*)malloc((size_t)(lineLength));
    
    // Shift the file pointer to the start of the line so that we can use
    // the fgets function and read lineLength characters to get the line string    
    if(fseek(stream, startCharIndex, SEEK_SET) != 0)
        return 0;
    // fgets appends a null-terminating character to the string automatically
    // so adding it manually is not necessarry
    // lineLength + 1 is required because fgets goes up to n-1st character in the stream,
    // therefore doing just lineLength would omit 1 character
    if(fgets(*out, lineLength + 1, stream) == NULL)
        return 0;
    
    *length = lineLength;
    return 1;
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
    while(jsonFileGetLine(&line, &lineLength, file) != 0)
    {
        int keyStartQuoteIndex = -1;
        int keyEndQuoteIndex = -1;
        char *key;

        for (size_t i = 0; i < lineLength; i++)
        {
            // WIP
            // switch(line[i])
            // {
            //     case '"':
            //     {
            //         // FIXME: Causes segfault because the indexes get overwritten to -1,
            //         // probably due to a wonky condition
            //         keyStartQuoteIndex = keyStartQuoteIndex == -1 ? i : -1;
            //         keyEndQuoteIndex = (keyEndQuoteIndex == -1) && (keyStartQuoteIndex != -1) ? i : -1; 
            //     }
            //     break;
            
            //     case ':':
            //     {
            //         size_t keySize = (keyEndQuoteIndex - 1) - (keyStartQuoteIndex + 1);
            //         key = (char*)malloc(keySize);
            //         strncpy(key, &line[keyStartQuoteIndex], keySize);
            //     }
            //     break;

            //     case '{':
            //     {
            //         if(line[i] == 0)
            //             continue;
            //     }
            //     break;
            //     case '}':
            //     {
            //         if(line[i] == lineLength)
            //             continue;
            //     }
            //     break;
            // }
        }
    }

    // char *line = NULL;
    // ssize_t lineLength = 0; 
    
    // while((lineLength = jsonFileGetLine(&line, NULL, file)) != -1)
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