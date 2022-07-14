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
#include "json-c_linked_list.h"

#include <stdlib.h>

// Macro to quickly determine if the index fits within the list's bounds
// The max value must be specified as list->size - 1 (for clarity in code)
#define IS_INDEX_OUT_OF_RANGE(i, max) (i < 0 || (i > max && i != 0))

int jsonLinkedListCreate(JSONLinkedList **listPtrPtr)
{
    if(listPtrPtr == NULL)
        return -1;
    
    JSONLinkedList *newList = (JSONLinkedList*)malloc(sizeof(JSONLinkedList));
    
    if(newList == NULL)
        return 0;
    
    newList->size = 0;
    newList->start = NULL;

    *listPtrPtr = newList;
    return 1;
}
int jsonLinkedListFree(JSONLinkedList **listPtrPtr, int freeValues)
{
    if(listPtrPtr == NULL || *listPtrPtr == NULL)
        return -1;
    
    JSONLinkedList *list = *listPtrPtr;

    // Trying to free nodes of an empty list leads to a segfault
    if(list->start != NULL)
    {
        // Stores the next node in the list so that it doesn't get lost
        // once the current node gets freed
        JSONLinkedListNode *nextNode = NULL;
        for (size_t i = 0; i < list->size; i++)
        {
            // Holds the pointer to the current node of the list
            JSONLinkedListNode *currentNode = NULL;
            
            // Ensures that the first iteration gets set to the 'start' pointer of JSONLinkedList
            if(i == 0)
            {
                currentNode = list->start; 
                nextNode = currentNode->next;
            }
            // Every other iteration should set the currentNode to the next node from the previous iteration
            // and update the nextNode to be able to repeat this whole shebang in the next iteration 
            else
            {
                currentNode = nextNode;
                nextNode = currentNode->next;
            }
            
            if(freeValues == 1) 
            {
                free(currentNode->data);
                currentNode->data = NULL;
            }
            free(currentNode);
            currentNode = NULL;
        }
    }
    
    free(list);
    list = NULL;

    *listPtrPtr = list;
    return 1;
}

int jsonLinkedListInsert(JSONLinkedList** const listPtrPtr, int index, void* const value)
{
    if(listPtrPtr == NULL || *listPtrPtr == NULL)
        return -1;
    if(value == NULL)
        return -1;

    JSONLinkedList *list = *listPtrPtr;

    // The function should be able to insert an element right after the last valid one,
    // hence the extra condition
    if(IS_INDEX_OUT_OF_RANGE(index, list->size - 1) && index != list->size)
        return 0;

    // Stores the node at the desired index in the list
    // so that the specified value can be inserted in it
    JSONLinkedListNode **desiredNode = NULL;
    
    // Skip looping through the list if the index is 0
    // because it's not necessarry seeing as 0 = start of list
    if(index == 0)
        desiredNode = &(list->start);
    else
    {
        // Because the function can insert data after the last valid index
        // it is necessary to ensure that an index with an as of yet empty node
        // doesn't get accessed and isn't attempted to be used, which could cause a segfault
        if(index < list->size)
        {
            // i can be equal to index as well because it's under the list's size
            // and is therefore guaranteed to have a node present
            for (size_t i = 0; i <= index; i++)
            {
                if(i == 0)
                    desiredNode = &(list->start);
                else
                    desiredNode = &(*desiredNode)->next;
            }
        }
        else
        {
            // i can't be equal to index because it's the list's size
            // and the node for said index is not initialized yet, therefore
            // the last initialized node must be accessed so that
            // a new node to be appended to it
            for (size_t i = 0; i < index; i++)
            {
                if(i == 0)
                    desiredNode = &(list->start);
                else
                    desiredNode = &(*desiredNode)->next;
            }
        }
    }

    // Free and replace the value of the node if a JSONLinkedListNode
    // is already present at the given index
    // Extra condition required so that the next node of the last valid node
    // isn't attempted to be dereferenced before being initialized
    if((desiredNode != NULL && *desiredNode != NULL) && index != list->size)
    {
        free((*desiredNode)->data);
        (*desiredNode)->data = value;
    }   
    // Otherwise create a new JSONLinkedListNode
    else
    {
        JSONLinkedListNode *newNode = (JSONLinkedListNode*)malloc(sizeof(JSONLinkedListNode));
        // Only append the new node if it was able to be allocated
        if(newNode == NULL)
        {
            free(newNode);
            return 0;
        }
        newNode->data = value;
        newNode->next = NULL;

        // The start ptr is always NULL on list create, so this check is needed to ensure 
        // that the new node gets set as the new start pointer properly
        if(*desiredNode == NULL)
            *desiredNode = newNode;
        else
            (*desiredNode)->next = newNode;
        
        list->size++;
    } 
    
    return 1;    
}
int jsonLinkedListReplace(JSONLinkedList** const listPtrPtr, int index, void* const value)
{
    if(listPtrPtr == NULL || *listPtrPtr == NULL)
        return -1;
    if(value == NULL)
        return -1;

    JSONLinkedList *list = *listPtrPtr;

    if(IS_INDEX_OUT_OF_RANGE(index, list->size - 1))
        return 0;

    // Stores the node at the desired index in the list
    // so that its data can be replaced
    JSONLinkedListNode **desiredNode = NULL;
    
    // Skip looping through the list if the index is 0
    // because it's not necessarry seeing as 0 = start of list
    if(index == 0)
        desiredNode = &(list->start);
    else
    {
        for (size_t i = 0; i <= index; i++)
        {
            if(i == 0)
                desiredNode = &(list->start);
            else
                desiredNode = &(*desiredNode)->next;
        }
    }

    // Replace the value of the node of the node
    // if it has been initialized already
    if(desiredNode != NULL && *desiredNode != NULL)
    {
        (*desiredNode)->data = value;
    }
    else
        return 0;
    
    return 1;
}
int jsonLinkedListRemove(JSONLinkedList** const listPtrPtr, void* const dataToRemove)
{
    if(listPtrPtr == NULL || *listPtrPtr == NULL)
        return -1;
    if(dataToRemove == NULL)
        return -1;

    JSONLinkedList *list = *listPtrPtr;

    // Trying to remove data from an empty list doesn't make sense
    if(list->size <= 0)
        return 0;

    // The current node in the list
    JSONLinkedListNode **currentNode = NULL;
    // The node before the one that's to be freed
    // Used to relink the whole list and keep it intact
    JSONLinkedListNode **lastNode = NULL;
    // Stores the node with the desired data
    // so that it can be removed later
    JSONLinkedListNode **desiredNode = NULL;

    // Save the index at which the node is found so that it can be
    // used later for determining where and how to relink the list
    int indexOfNode = 0;
    // Loop through the list and look for data that should be removed
    for (size_t i = 0; i <= list->size - 1; i++)
    {
        // Break out of the loop if the desired node has been found
        if(desiredNode != NULL)
            break;

        if(i == 0)
        {
            currentNode = &(list->start);
        }
        else
        {
            lastNode = currentNode;
            currentNode = &(*lastNode)->next;
        }
        
        if((*currentNode)->data == dataToRemove)
            desiredNode = currentNode;

        indexOfNode = i;
    }

    // Attempting to free a NULL ptr would lead to an error
    // Conveniently, it also means that the data isn't present
    // in this list so we can return the hell out of here
    if(desiredNode == 0 || *desiredNode == NULL)
        return 0;
    else
    {
        JSONLinkedListNode *nextNode = (*desiredNode)->next;
        
        // Free the desired data and the list node
        free((*desiredNode)->data);
        free((*desiredNode));

        // Make sure to replace the start node with the next node
        // if the index is 0
        if(indexOfNode == 0)
        {
            list->start = nextNode;
        }
        // If it's not 0, make the node before the one that was just deleted
        // point to the just deleted node's next node
        else
        {
            (*lastNode)->next = nextNode;
        }

        list->size--;
    }    

    return 1;
}
int jsonLinkedListRemoveAt(JSONLinkedList** const listPtrPtr, int index)
{
    if(listPtrPtr == NULL || *listPtrPtr == NULL)
        return -1;
    
    JSONLinkedList *list = *listPtrPtr;

    if(list->size == 0)
        return 0;

    if(IS_INDEX_OUT_OF_RANGE(index, list->size - 1))
        return 0;

    // Stores the node at the desired index in the list
    // so that it can be removed later
    JSONLinkedListNode **desiredNode = NULL;
    // The node before the one that's to be freed
    // Used to relink the whole list and keep it intact
    JSONLinkedListNode **lastNode = NULL;
    
    // Skip looping through the list if the index is 0
    // because it's not necessarry seeing as 0 = start of list
    if(index == 0)
    {
        desiredNode = &(list->start);
    }
    else
    {
        for (size_t i = 0; i <= index; i++)
        {
            if(i == 0)
                desiredNode = &(list->start);
            else
            {
                lastNode = desiredNode;
                desiredNode = &(*desiredNode)->next;
            }
        }
    }

    // Attempting to free a NULL ptr would lead to an error
    if(desiredNode == NULL || *desiredNode == NULL)
        return 0;
    else
    {
        JSONLinkedListNode *nextNode = (*desiredNode)->next;
        
        free((*desiredNode)->data);
        free((*desiredNode));

        // Make sure to replace the start node with the next node
        // if the index is 0
        if(index == 0)
        {
            list->start = nextNode;
        }
        // If it's not 0, make the last node point to the about-to-be-removed's next node
        else
        {
            (*lastNode)->next = nextNode;
        }

        list->size--;
    }

    return 1;
}

// TODO: REFACTOR 
int jsonLinkedListPushFront(JSONLinkedList** const listPtrPtr, void* const value)
{
    JSONLinkedList *list = *listPtrPtr;
    if(list == NULL)
        return -1;
    
    // Create a new node from the provided value
    JSONLinkedListNode *newNode = (JSONLinkedListNode*)malloc(sizeof(JSONLinkedListNode));
    // Only append the new node if it was able to be allocated
    if(newNode == NULL)
    {
        free(newNode);
        return 0;
    }
    newNode->data = value;
    newNode->next = NULL;
    
    // Replace the start node with a new node
    // If the start is NULL, we can just set it to the new node directly
    // because no sort of relinking the list is necessary
    if(list->start == NULL)
        list->start = newNode;
    else
    {
        // Store the current start node
        JSONLinkedListNode *oldStartNode = list->start;
        // Set the new start node's next ptr to the previous start node
        list->start = newNode;
        newNode->next = oldStartNode;
    }
    
    list->size++;
    return 1;    
}
// TODO: REFACTOR 
int jsonLinkedListPopFront(JSONLinkedList **listPtrPtr)
{
    JSONLinkedList *list = *listPtrPtr;
    if(list == NULL)
        return -1;

    if(list->size == 0)
        return 0;

    JSONLinkedListNode *nextNode = list->start->next;

    free(list->start->data);
    free(list->start);

    list->start = nextNode;

    list->size--;
    return 1;
}

// TODO: REFACTOR 
int jsonLinkedListPushBack(JSONLinkedList** const listPtrPtr, void* const value)
{
    JSONLinkedList *list = *listPtrPtr;
    if(list == NULL)
        return -1;
    
    // Stores the last node of the list so that a new node can be placed after it
    JSONLinkedListNode **lastNode = NULL;
    // The last node is the start node of the list if the list is empty
    if(list->size == 0)
        lastNode = &(list->start);
    // Otherwise the last node has to be reached by going through each node
    // and setting the last node as the current last node's 'next' ptr
    else
    {
        for (size_t i = 0; i < list->size; i++)
        {
            if(i == 0)
                lastNode = &(list->start);
            else
                lastNode = &(*lastNode)->next;
        }
    }
        
    JSONLinkedListNode *newNode = (JSONLinkedListNode*)malloc(sizeof(JSONLinkedListNode));
    // Only append the new node if it was able to be allocated
    if(newNode == NULL)
    {
        free(newNode);
        return 0;
    }
    newNode->data = value;
    newNode->next = NULL;

    // The start ptr is always NULL, so this check is needed to ensure 
    // that the new node gets set as the new start pointer properly
    if(*lastNode == NULL)
        *lastNode = newNode;
    else
        (*lastNode)->next = newNode;
    
    list->size++;
    return 1;    
}
// TODO: REFACTOR 
int jsonLinkedListPopBack(JSONLinkedList** const listPtrPtr)
{
    JSONLinkedList *list = *listPtrPtr;
    if(list == NULL)
        return -1;

    // It doesn't make sense for an element to be removed from an empty list    
    if(list->size == 0)
        return 0;

    // Stores the last node of the list so that it can be removed later
    JSONLinkedListNode **lastNode = NULL;
    for (size_t i = 0; i < list->size; i++)
    {
        if(i == 0)
            lastNode = &(list->start);
        else
            lastNode = &(*lastNode)->next;
    }

    free((*lastNode)->data);
    (*lastNode)->data = NULL;
    free(*lastNode);
    *lastNode = NULL;  
    
    list->size--;
    return 1;    
}

int jsonLinkedListContains(JSONLinkedList** const listPtrPtr, void* const value)
{
    if(listPtrPtr == NULL || *listPtrPtr == NULL)
        return -1;
    if(value == NULL)
        return -1;

    JSONLinkedList* const list = *listPtrPtr;
    
    if(list->size == 0)
        return 0;

    // No need to loop through the list if there's only one element in it
    if(list->size == 1)
        return list->start->data == value;
    else
    {
        // Save the current node so that the value ptrs can be compared
        JSONLinkedListNode **currentNode;
        for (size_t i = 0; i <= list->size; i++)
        {
            if(i == 0)
                currentNode = &(list->start);
            else
                currentNode = &((*currentNode)->next);

            if(*currentNode == NULL) 
                break; 
            else
                return (*currentNode)->data == value;
        }
    }

    return 0;
}

// TODO: REFACTOR 
int _jsonLinkedListAt(JSONLinkedList** const listPtrPtr, int index, void** outValue)
{
    JSONLinkedList* const list = *listPtrPtr;

    if(list == NULL)
        return -1;

    if(IS_INDEX_OUT_OF_RANGE(index, list->size - 1))
        return 0;

    // Stores the node at the desired index in the list
    // so that the specified value can be retrieved from it
    JSONLinkedListNode** desiredNode = NULL;
    
    // Skip looping through the list if the index is 0
    // because it's not necessarry seeing as 0 = start of list
    if(index == 0)
        desiredNode = &(list->start);
    else
    {
        for (size_t i = 0; i <= index; i++)
        {
            if(i == 0)
                desiredNode = &(list->start);
            else
                desiredNode = &(*desiredNode)->next;
        }
    }

    if(*desiredNode == NULL)
        return 0;
    else
        *outValue = (*desiredNode)->data;
}

// TODO: REFACTOR 
int _jsonLinkedListToArray(JSONLinkedList** const listPtrPtr, void*** outArray)
{
    JSONLinkedList *list = *listPtrPtr;

    void **array = (void**)malloc(list->size * sizeof(void*));

    JSONLinkedListNode **currentNode;
    for (size_t i = 0; i < list->size; i++)
    {
        if(i == 0)
            currentNode = &(list->start);
        else
            currentNode = &((*currentNode)->next);

        array[i] = (*currentNode)->data;
    }

    *outArray = array;
}