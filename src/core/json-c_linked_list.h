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

If the provided list ptr is not NULL, 
the function frees the currently present list 
and replaces the old list with an empty one

THE PASSED IN LIST PTR MUST BE SET TO NULL WHEN FIRST DECLARING THE LIST VAR
like so:
JSONLinkedList *list = NULL;
jsonCreateLinkedList(&list);

Returns: 
1 -- successfully allocated a new empty list
0 -- allocation was unsuccessful
*/
int jsonLinkedListCreate(JSONLinkedList **listPtrPtr);
/*
Frees the provided list and all of its nodes
Returns: 
 1 -- all nodes were freed correctly
 0 -- some nodes were not freed
-1 -- provided list is uninitialized
*/
int jsonLinkedListFree(JSONLinkedList **listPtrPtr);

/*
Checks whether some node in the provided list contains the desired value
Returns: 
 1 -- value was found
 0 -- value was NOT found
-1 -- provided list is uninitialized
*/
int jsonLinkedListContains(JSONLinkedList** const listPtrPtr, void *value);

/*
Retrieves the value of a node at the provided index
Returns:
 1 -- index was found and a value was successfully retrieved,
 0 -- node at the desired index doesn't exist,
-1 -- provided list is uninitialized
*/
int _jsonLinkedListAt(JSONLinkedList** const listPtrPtr, int index, void** outValue);
#define jsonLinkedListAt(listPtrPtr, index, outValue, T) \
{ \
    void* temp = NULL; \
    _jsonLinkedListAt(listPtrPtr, index, &temp); \
    outValue = *((T*)temp); \
} \

/*
Inserts the specified value into a node at the desired index
Returns:
 1 -- value inserted successfully
 0 -- failed inserting value
-1 -- provided list is uninitialized
*/
int jsonLinkedListInsert(JSONLinkedList** const listPtrPtr, int index, const void* const value);
/*
Removes and frees the node at the desired index
Returns:
 1 -- node removed successfully
 0 -- failed removing node
-1 -- provided list is uninitialized
*/
int jsonLinkedListRemove(JSONLinkedList** const listPtrPtr, int index);

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