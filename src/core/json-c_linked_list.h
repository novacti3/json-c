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

// TODO: Do a pass on all of the comments and go into more detail 
//       about what makes the funcs return what error code

#ifndef JSON_C_LINKED_LIST
#define JSON_C_LINKED_LIST

// Forward declarations of the struct so that
// they can be used to init fields in their definitions
typedef struct JSONLinkedList JSONLinkedList;
typedef struct JSONLinkedListNode JSONLinkedListNode;

// Represents a linked list data structure of n nodes
typedef struct JSONLinkedList
{
    int size;
    JSONLinkedListNode *start;
} JSONLinkedList;

// Represents a single node in a linked list data structure
typedef struct JSONLinkedListNode
{
    JSONLinkedListNode *next;
    void *data;
} JSONLinkedListNode;

/*
Creates a new linked list

WARNING: 
The function does NOT free the provided ptr if a list is already initiated
Before calling jsonLinkedListCreate on a ptr that had been used for a list before,
ensure that the list was freed using jsonLinkedListFree

Returns: 
 1 -- successfully allocated a new empty list
 0 -- allocation was unsuccessful
-1 -- invalid list Ptr Ptr provided
*/
int jsonLinkedListCreate(JSONLinkedList **listPtrPtr);
/*
Frees the provided list and all of its nodes
This function is guaranteed to succeed if a valid list is provided

If 'freeValues' parameter is set to 1, the data held by the list nodes will be freed as well

Returns: 
 1 -- all nodes were freed correctly
-1 -- provided list is uninitialized
*/
int jsonLinkedListFree(JSONLinkedList **listPtrPtr, int freeValues);

/*
Inserts the specified value into a node at the desired index
Index must be greater than 0 and less than the list's current size + 1
eg. indexes valid for a list with the size of 1: 0, 1

WARNING:
If data is already present at the given index, the old data is freed
in favor of the new data. If you don't want that to happen,
use the 'jsonLinkedListReplace' function, which doesn't free the old data

Returns:
 1 -- value inserted successfully
 0 -- failed inserting value or index was out of range
-1 -- provided list is uninitialized or the provided value is NULL
*/
int jsonLinkedListInsert(JSONLinkedList** const listPtrPtr, int index, void* const value);
/*
Replaces the data at the provided index with new data
The old data isn't freed, therefore it is safe to use even after calling this function

Returns:
 1 -- data replaced successfully
 0 -- failed replacing data, index out of range
-1 -- provided list is uninitialized or the provided value is NULL
*/
int jsonLinkedListReplace(JSONLinkedList** const listPtrPtr, int index, void* const value);
/*
Searches for the desired data and removes it and frees it from the list if it's present

Returns:
 1 -- node removed successfully
 0 -- failed removing node, desired data not present in the list or the list is empty
-1 -- provided list is uninitialized or the provided data to look for is NULL
*/
// NOTE: Because the ptr is copied, there is no need for having an option for whether data should be freed or not
int jsonLinkedListRemove(JSONLinkedList** const listPtrPtr, void* const dataToRemove);
/*
Removes and frees the node at the desired index

Returns:
 1 -- node removed successfully
 0 -- failed removing node, index out of range or list is empty
-1 -- provided list is uninitialized
*/
// NOTE: Because the ptr is copied, there is no need for having an option for whether data should be freed or not
int jsonLinkedListRemoveAt(JSONLinkedList** const listPtrPtr, int index);

/*
Inserts a value to the front of the list
Returns:
 1 -- value inserted successfully
 0 -- failed inserting value
-1 -- provided list is uninitialized
*/
int jsonLinkedListPushFront(JSONLinkedList** const listPtrPtr, void* const value);
/*
Removes the value at the beginning of the list
Returns:
 1 -- value removed successfully
 0 -- failed removing value
-1 -- provided list is uninitialized
*/
int jsonLinkedListPopFront(JSONLinkedList** const listPtrPtr);

/*
Inserts a value to the end of the list
Returns:
 1 -- value inserted successfully
 0 -- failed inserting value
-1 -- provided list is uninitialized
*/
int jsonLinkedListPushBack(JSONLinkedList** const listPtrPtr, void* const value);
/*
Removes the value at the end of the list
Returns:
 1 -- value removed successfully
 0 -- failed removing value
-1 -- provided list is uninitialized
*/
int jsonLinkedListPopBack(JSONLinkedList** const listPtrPtr);

/*
Checks whether some node in the provided list contains the desired value

Returns: 
 1 -- value was found
 0 -- value was not found
-1 -- provided list is uninitialized
*/
int jsonLinkedListContains(JSONLinkedList** const listPtrPtr, void* const value);

int _jsonLinkedListAt(JSONLinkedList** const listPtrPtr, int index, void** outValue);
/*
Retrieves the value of a node at the provided index as a reference
Returns:
 1 -- index was found and a value was successfully retrieved
 0 -- node at the desired index doesn't exist
-1 -- provided list is uninitialized
*/
// FIXME: Add param for func result
#define jsonLinkedListAt(listPtrPtr, index, outValue, T) \
{ \
    void* temp = NULL; \
    _jsonLinkedListAt(listPtrPtr, index, &temp); \
    outValue = *((T*)temp); \
} \
/*
Retrieves the value of a node at the provided index as a pointer
Returns:
 1 -- index was found and a value was successfully retrieved
 0 -- node at the desired index doesn't exist
-1 -- provided list is uninitialized
*/
// FIXME: Figure out a way to make funcResult an optional parameter
// FIXME: funcResult still doesn't work properly
#define jsonLinkedListAtPtr(listPtrPtr, index, funcResultPtr, outValue, Tptr) \
{ \
    void *temp = NULL; \
    *funcResultPtr = _jsonLinkedListAt(listPtrPtr, index, &temp); \
    outValue = (Tptr)temp; \
} \

/*
Converts the list into a regular C-style array
Returns:
 1 -- array created successfully
 0 -- failed creating array
-1 -- provided list is uninitialized
*/
int _jsonLinkedListToArray(JSONLinkedList** const listPtrPtr, void*** outArray);
#define jsonLinkedListToArray(listPtrPtr, outArray, T) \
{ \
    void **tempArray = NULL; \
    _jsonLinkedListToArray(listPtrPtr, &tempArray); \
    outArray = (T**)tempArray; \
} \

#endif