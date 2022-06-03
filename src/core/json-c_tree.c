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
#include "json-c_tree.h"

#include <stdlib.h>

int jsonTreeCreate(JSONTree **treePtrPtr)
{
    if(*treePtrPtr != NULL)
        jsonTreeFree(treePtrPtr, 1);
    
    JSONTree *newTree = (JSONTree*)malloc(sizeof(JSONTree));
    if(newTree == NULL)
        return 0;
    
    newTree->nodes = NULL;
    if(jsonLinkedListCreate(&(newTree->nodes)) == 0)
        return 0;

    *treePtrPtr = newTree;
    return 1;
}
int jsonTreeFree(JSONTree **treePtrPtr, int freeValues)
{
    JSONTree *tree = *treePtrPtr;

    if(tree == NULL)
        return -1;
    if(tree->nodes == NULL)
        return 0;

    if(jsonLinkedListFree(&(tree->nodes), freeValues))
        return 0;
    else
        return 1;
}

int jsonTreeCreateNode(JSONTreeNode **out, char* key, JSONValue value)
{
    if(out == NULL)
        return -1;
    
    JSONTreeNode *outNode = *out;
    outNode = (JSONTreeNode*)malloc(sizeof(JSONTreeNode));
    outNode->info = (JSONPair*)malloc(sizeof(JSONPair));
    outNode->info->key = key;
    outNode->info->value = value;
    outNode->childNodes = NULL;
    jsonLinkedListCreate(&(outNode->childNodes));

    *out = outNode;

    return 1;
}

int jsonTreeInsert(JSONTree **treePtrPtr, JSONTreeNode **nodePtrPtr, JSONTreeNode **parentNodePtrPtr)
{
    JSONTree *tree = *treePtrPtr;
    JSONTreeNode *node = *nodePtrPtr;
    
    if(tree == NULL)
        return -1;
    if(node == NULL)
        return 0;

    if(parentNodePtrPtr == NULL)
    {
        jsonLinkedListPushBack(&(tree->nodes), (void*)node);
    }
    else
    {
        JSONTreeNode *parentNode = *parentNodePtrPtr;
        jsonLinkedListPushBack(&(parentNode->childNodes), (void*)node);
    }
    
    return 1;
}
int jsonTreeRemove(JSONTreeNode **nodePtrPtr, int freeChildren)
{

}