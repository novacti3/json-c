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
#include <string.h>

// Forward declarations
static int _jsonFreeStringArray(char ***array, const int* const length);
// ----------

int _jsonFileGetLine(char **out, int *length, FILE *stream)
{
    // NOTE: Should the function worry about freeing the out pointer?
    if(out != NULL)
    {
        free(*out);
        free(out);
    }

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

// Splits the source string using the provided delim character into an array of n char*
int _jsonSplitString(char ***dest, const char* const src, const char delim, int* const numOfStrings)
{
    // Array of char*
    char **stringsArray = *dest;
    int newStringsAmount = 0;

    // NOTE: Freeing here might actually goof up the keys and value ptrs
    //       in the previous JSONPairs, because it'd free them up. 
    //       Needs investigation.
    // Free the passed in array to avoid memory leaks
    // because it's going to get reassigned to a newly created array
    // that has the amount of elements required 
    // if(stringsArray != NULL)
    // {
    //     _jsonFreeStringArray(&stringsArray, numOfStrings);
    // }
    stringsArray = NULL;
    *numOfStrings = 0;

    // Used to determine the span of new strings inside of the source string
    int newStringStartIndex = -1;
    int newStringEndIndex = -1;

    // Go through each character in the source string 
    // until the provided delimination character is encountered
    int i = 0;
    char c = '\0';
    // Used to determine whether or not a string could be split
    // and therefore whether the end of the line/end of the string
    // should be added to the array of strings 
    int wasDelimEncountered = 0;
    do
    {
        c = src[i];

        // Use this character as the start of a new string
        // if the construction of another string isn't already in progress
        if(newStringStartIndex == -1)
            newStringStartIndex = i;

        // Only create a string from the end of the source string if the delim char
        // has already been encountered, because there is no need to split the string
        // if the character that was supposed to split it hasn't been encountered
        int createStringFromEndOfLine = ((c == '\n' || c == '\0') && wasDelimEncountered);
        if(c == delim || createStringFromEndOfLine)
        {
            if(stringsArray == NULL)
                stringsArray = (char**)malloc(sizeof(char**));
            
            wasDelimEncountered = 1;
            
            // Use this character as the end of the string that's currently being constructed
            // because the desired delim char (or end of file) has been reached and therefore
            // we have the entire span we need for a new string
            if(newStringEndIndex == -1)
                newStringEndIndex = i;

            int newStringLength = newStringEndIndex - newStringStartIndex;
            // Leave space at the end of the new string for a null-termination char,
            // because strncpy doesn't append it to the end automatically
            char *newString = (char*)malloc((newStringLength + 1) * sizeof(char));
            
            // Copy from src + startIndex for the required length into a new string,
            // because src is a pointer to the first char in the string
            // and startIndex is the offset from the start at which the new string starts
            strncpy(newString, src + newStringStartIndex, (size_t)(newStringLength));
            // Because of array indexes starting from 0,
            // a str of length 4 would have the indexes 0-3
            // and length leads directly to extra allocated char 
            // for null-termination char
            newString[newStringLength] = '\0';

            if(newStringsAmount == 0)
            {
                // If it's the first new string, there isn't any need to allocate
                // room for it, because it was already allocated when creating the new strings array
                stringsArray[0] = newString;
            }
            else
            {        
                // If other strings have already been added, 
                // the array needs to be grown to accomodate 
                // for the newly created string
                
                // 1: Allocate a new array with +1 length than the current array
                //    to accomodate for the new string
                char **resizedStringsArray = (char**)malloc((newStringsAmount + 1) * sizeof(char*));
                
                // 2: Copy the contents of the current array into the resized array
                memmove(resizedStringsArray, stringsArray, (newStringsAmount + 1) * sizeof(char*));
                // 3: Add the new string as the last element of the new array
                // NOTE: It's just newStringsAmount (not amount - 1 or +1)
                //       because arrays are indexed from 0, therefore doing that
                //       would lead to trying to assign to an address out of bounds
                resizedStringsArray[newStringsAmount] = newString;

                // NOTE: Freeing the entire old array causes all but the new string
                //       to be garbage data because it frees the actual char*s containing
                //       the string data. Needs investigation.
                // 4: Free the old array because it's going to get replaced by the new one
                // _jsonFreeStringArray(&stringsArray, &newStringsAmount);
                free(stringsArray);

                // 5: Replace the old array with the new resized one
                stringsArray = resizedStringsArray;
            }

            newStringsAmount++;

            newStringStartIndex = i + 1;
            newStringEndIndex = -1;
        }

        i++;
    } while (c != '\0' && c != '\n');

    *dest = stringsArray;
    *numOfStrings = newStringsAmount;
    if(newStringsAmount <= 0)    
        return 0;
    else
        return 1;
}

static int _jsonFreeStringArray(char ***array, const int* const length)
{
    if(*array != NULL)
    {
        for (size_t i = 0; i < *length; i++)
        {
            char *str = (*array)[i];
            free(str);
        }
        free(*array);
        *array = NULL;

        return 1;
    }
    else
        return 0;
}