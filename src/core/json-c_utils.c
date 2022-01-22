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
#include "json-c_utils.h"

#include <stdlib.h>

int jsonFileGetLine(char **out, int *length, FILE *stream)
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

        // Don't run the loop if end-of-file was reached because there's no point to work with the character further
        if(c == EOF)
            continue;

        // Ignore whitespaces and tabs before the actual line content starts
        // because there's no need to include them in the output
        if(firstIter && (c != ' ' && c != '\t'))
        {
            startCharIndex = ftell(stream) - 1;
            firstIter = 0;
        }

        // Not using a switch statement here because we need to be
        // able to break out of the loop, whereas break in a switch statement
        // only ends the case
        if (c =='\n')
        {
            endCharIndex = ftell(stream);
            break;
        }
    }

    // No line needs to be saved because end-of-file was reached 
    if(endCharIndex == -1)
        return 0;


    int lineLength = endCharIndex - startCharIndex;
    // Allocate enough space for all of the characters on the line
    // Because fgets gets a string up to n-1 characters, the last character is reserved
    // for the null-terminating char, so allocating lineLength + 1 to set aside space for it is not needed
    *out = (char*)malloc((size_t)(lineLength));
    
    // Shift the file pointer to the start of the line so that we can use
    // the fgets function and read lineLength characters to get the line string    
    if(fseek(stream, startCharIndex, SEEK_SET) != 0)
        return 0;
    // fgets appends a null-terminating character to the string automatically
    // so adding it manually is not necessarry
    // FIXME: Make sure that the newline char at the end of the line
    // won't be present in the output string
    if(fgets(*out, lineLength, stream) == NULL)
        return 0;
    
    *length = lineLength;
    return 1;
}