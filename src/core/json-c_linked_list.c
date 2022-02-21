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

#define IS_INDEX_OUT_OF_RANGE(i, max) (i < 0 || i >= max)

int jsonLinkedListCreate(JSONLinkedList **list)
{
    // Free the provided pointer to avoid memory leaks
    // FIXME: Throws a segfault if a list ptr not set to NULL as default is passed in
    //        While this is technically correct behaviour, it's absolutely shit from a usage perspective
    if(*list != NULL)
        jsonLinkedListFree(list);
    
    JSONLinkedList *newList = (JSONLinkedList*)malloc(sizeof(JSONLinkedList));
    
    if(newList == NULL)
        return 0;
    
    newList->size = 0;
    newList->start = NULL;

    *list = newList;
    return 1;
}
int jsonLinkedListFree(JSONLinkedList **list)
{
    JSONLinkedList *listToDelete = *list;
    if(listToDelete == NULL)
        return -1;

    // Stores the next node in the list so that it doesn't get lost
    // once the current node gets freed
    JSONLinkedListNode *nextNode = NULL;
    for (size_t i = 0; i < listToDelete->size; i++)
    {
        // Holds the pointer to the current node of the list
        JSONLinkedListNode *currentNode = NULL;
        
        // Ensures that the first iteration gets set to the 'start' pointer of JSONLinkedList
        if(i == 0)
        {
            currentNode = listToDelete->start; 
            nextNode = currentNode->next;
        }
        // Every other iteration should set the currentNode to the next node from the previous iteration
        // and update the nextNode to be able to repeat this whole shebang in the next iteration 
        else
        {
            currentNode = nextNode;
            nextNode = currentNode->next;
        }
        
        free(currentNode->data);
        free(currentNode);
    }
    free(listToDelete);
    listToDelete = NULL;

    if(listToDelete == NULL)
    {
        *list = listToDelete;
        return 1;
    }
    else
        return 0;
}

int jsonLinkedListContains(JSONLinkedList** const list, void *value)
{

}

int jsonLinkedListAt(JSONLinkedList** const list, int index, void** outValue)
{
    JSONLinkedList* const linkedList = *list;

    if(linkedList == NULL)
        return -1;

    if(!IS_INDEX_OUT_OF_RANGE(index, linkedList->size - 1))
        return 0;

    // Stores the node at the desired index in the list
    // so that the specified value can be retrieved from it
    JSONLinkedListNode** desiredNode = NULL;
    
    // Skip looping through the list if the index is 0
    // because it's not necessarry seeing as 0 = start of list
    if(index == 0)
        desiredNode = &(linkedList->start);
    else
    {
        for (size_t i = 0; i <= index; i++)
        {
            if(i == 0)
                desiredNode = &(linkedList->start);
            else
                desiredNode = &(*desiredNode)->next;
        }
    }

    if(*desiredNode == NULL)
        return 0;
    else
        *outValue = (*desiredNode)->data;
}
int jsonLinkedListInsert(JSONLinkedList** const list, int index, const void* const value)
{
    JSONLinkedList *linkedList = *list;

    if(linkedList == NULL)
        return -1;

    if(!IS_INDEX_OUT_OF_RANGE(index, linkedList->size - 1))
        return 0;

    // Stores the node at the desired index in the list
    // so that the specified value can be inserted in it
    JSONLinkedListNode **desiredNode = NULL;
    
    // Skip looping through the list if the index is 0
    // because it's not necessarry seeing as 0 = start of list
    if(index == 0)
        desiredNode = &(linkedList->start);
    else
    {
        for (size_t i = 0; i <= index; i++)
        {
            if(i == 0)
                desiredNode = &(linkedList->start);
            else
                desiredNode = &(*desiredNode)->next;
        }
    }

    // Just replace the value of the node if a JSONLinkedListNode
    // is already present at the given index
    if(*desiredNode != NULL)
    {
        // NOTE: Could not freeing the value here lead to a mem leak?
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

        // The start ptr is always NULL, so this check is needed to ensure 
        // that the new node gets set as the new start pointer properly
        if(*desiredNode == NULL)
            *desiredNode = newNode;
        else
            (*desiredNode)->next = newNode;
        
        linkedList->size++;
    } 

    
    return 1;    
}
int jsonLinkedListRemove(JSONLinkedList** const list, int index)
{
    JSONLinkedList *linkedList = *list;

    if(linkedList == NULL)
        return -1;

    if(!IS_INDEX_OUT_OF_RANGE(index, linkedList->size - 1))
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
        desiredNode = &(linkedList->start);
    }
    else
    {
        for (size_t i = 0; i <= index; i++)
        {
            if(i == 0)
                desiredNode = &(linkedList->start);
            else
            {
                lastNode = desiredNode;
                desiredNode = &(*desiredNode)->next;
            }
        }
    }

    // Attempting to free a NULL ptr would lead to an error
    if(*desiredNode == NULL)
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
            linkedList->start = nextNode;
        }
        // If it's not 0, make the last node point to the about-to-be-removed's next node
        else
        {
            (*lastNode)->next = nextNode;
        }

        
        linkedList->size--;
        return 1;
    }
}

int jsonLinkedListPushFront(JSONLinkedList** const list, void* const value)
{

}
int jsonLinkedListPopFront(JSONLinkedList **list)
{

}

int jsonLinkedListPushBack(JSONLinkedList** const list, void* const value)
{
    JSONLinkedList *linkedList = *list;
    if(linkedList == NULL)
        return -1;
    
    // Stores the last node of the list so that a new node can be placed after it
    JSONLinkedListNode **lastNode = NULL;
    // The last node is the start node of the list if the list is empty
    if(linkedList->size == 0)
        lastNode = &(linkedList->start);
    // Otherwise the last node has to be reached by going through each node
    // and setting the last node as the current last node's 'next' ptr
    else
    {
        for (size_t i = 0; i < linkedList->size; i++)
        {
            if(i == 0)
                lastNode = &(linkedList->start);
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
    
    linkedList->size++;
    return 1;    
}
int jsonLinkedListPopBack(JSONLinkedList** const list)
{
    JSONLinkedList *linkedList = *list;
    if(linkedList == NULL)
        return -1;

    // It doesn't make sense for an element to be removed from an empty list    
    if(linkedList->size == 0)
        return 0;

    // Stores the last node of the list so that it can be removed later
    JSONLinkedListNode **lastNode = NULL;
    for (size_t i = 0; i < linkedList->size; i++)
    {
        if(i == 0)
            lastNode = &(linkedList->start);
        else
            lastNode = &(*lastNode)->next;
    }

    free((*lastNode)->data);
    (*lastNode)->data = NULL;
    free(*lastNode);
    *lastNode = NULL;  
    
    linkedList->size--;
    return 1;    
}

int jsonLinkedListToArray(JSONLinkedList** const list, void** outArray)
{
    
}