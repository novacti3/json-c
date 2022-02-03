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

typedef struct JSONLinkedList
{
    int size;
    JSONLinkedListNode *start;
} JSONLinkedList;

typedef struct JSONLinkedListNode
{
    JSONLinkedListNode *next;
    void *data;
} JSONLinkedListNode;

// Creates a new linked list
// Returns 1 if successful, 0 if unsuccessful
int jsonLinkedListCreate(JSONLinkedList **list);
// Frees the provided list and all of its nodes
// Returns 1 if all nodes were freed correctly, 0 if not, -1 if the list is uninitialized
int jsonLinkedListFree(JSONLinkedList **list);

// Checks whether some node in the provided list contains the desired value
// Returns 1 if the value was found, 0 if not, -1 if the list is uninitialized
int jsonLinkedListContains(const JSONLinkedList* const list, void *value);

// Retrieves the value of a node at the provided index
// Returns 1 if the index was found and a value was successfully retrieved,
// 0 if a node at the desired index doesn't exist,
// -1 if the list is uninitialized
int jsonLinkedListAt(const JSONLinkedList* const list, int index, const void* const outValue);
// Inserts the specified value into a node at the desired index
// Returns 1 if inserted successfully, 0 if not, -1 if list is uninitialized
int jsonLinkedListInsert(JSONLinkedList* const list, int index, const void* const value);
// Removes and frees the node at the desired index
// Returns 1 if removed successfully, 0 if not, -1 if list is uninitialized
int jsonLinkedListRemove(JSONLinkedList* const list, int index);

// Inserts the value to the front of the list
// Returns 1 if successful, 0 if not, -1 if list is uninitialized
int jsonLinkedListPushFront(JSONLinkedList* const list, void** const value);
// Removes the value at the beginning of the list
// Returns 1 if successful, 0 if not, -1 if list is uninitialized
int jsonLinkedListPopFront(JSONLinkedList *list);

// Inserts the value to the end of the list
// Returns 1 if successful, 0 if not, -1 if list is uninitialized
int jsonLinkedListPushBack(JSONLinkedList** const list, void** const value);
// Removes the value at the end of the list
// Returns 1 if successful, 0 if not, -1 if list is uninitialized
int jsonLinkedListPopBack(JSONLinkedList** const list);

// Converts the list into a regular C-style array
// Returns 1 if successful, 0 if not, -1 if list is uninitialized
int jsonLinkedListToArray(const JSONLinkedList* const list, void* outArray);

#endif