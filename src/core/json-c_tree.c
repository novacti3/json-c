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
#include <string.h>

int jsonTreeCreate(JSONTree **treePtrPtr)
{
    if (treePtrPtr == NULL)
        return -1;

    if (*treePtrPtr != NULL)
        jsonTreeFree(treePtrPtr);

    JSONTree *newTree = (JSONTree*)malloc(sizeof(JSONTree));
    if (newTree == NULL)
        return 0;

    if(jsonTreeCreateNode(&newTree->root, "root", (JSONValue){.type = JSON_VALUE_TYPE_NULL, .value = NULL}) == 0)
    {
        free(newTree);
        return 0;
    }
    
    *treePtrPtr = newTree;
    return 1;
}
int jsonTreeFree(JSONTree **treePtrPtr)
{
    if (treePtrPtr == NULL)
        return -1;

    JSONTree *tree = *treePtrPtr;

    if (tree == NULL)
        return -1;
    if (tree->root == NULL)
        return -1;

    JSONTreeNode *rootNode = tree->root;

    int listAtFuncResult;
    int freeFuncResult;
    for (size_t i = 0; i < rootNode->childNodes->size; i++)
    {
        JSONTreeNode *child = NULL;
        jsonLinkedListAtPtr(&(rootNode->childNodes), i, &listAtFuncResult, child, JSONTreeNode*); 
        
        freeFuncResult = jsonTreeFreeNode(&child, 1); 
        if(freeFuncResult == 0)
            return 0;
    }
    
    /* NOTE: 
        Repeat of code from jsonTreeFreeNode in order
        to have it be a more universal func 
        
        Originally, jsonTreeFreeNode(rootNode) was used here.
        Because of the previous loop, all of the root's children
        were freed already and freeing them again lead to segfaults.
        This introduced the need to include a condition in jsonTreeFreeNode 
        while checking if the node has any children, making sure that the desired node isn't root, 
        which defeated the purpose of jsonTreeFreeNode being a "universal" tree node disposal function.
    */
    // NOTE: freeNodes param doesn't affect anything here (as it does in jsonTreeFreeNode)
    //       because the root node is always guaranteed to be the same across every tree,
    //       is created upon tree creation and should therefore be freed every time to prevent waste
    free(rootNode->info->key);
    rootNode->info->key = NULL;
    free(rootNode->info->value.value);
    rootNode->info->value.value = NULL;
    free(rootNode->info);
    rootNode->info = NULL;
    jsonLinkedListFree(&(rootNode->childNodes), 1);
    free(rootNode);
    rootNode = NULL;
    
    free(tree);
    tree = NULL;

    *treePtrPtr = tree;
    return 1;
}

int jsonTreeCreateNode(JSONTreeNode **out, char *key, JSONValue value)
{
    if (out == NULL)
        return -1;

    JSONTreeNode *outNode = *out;

    outNode = (JSONTreeNode *)malloc(sizeof(JSONTreeNode));
    if(outNode == NULL)
        return 0;
    
    outNode->info = (JSONPair *)malloc(sizeof(JSONPair));
    if(outNode->info == NULL)
        return 0;
    
    outNode->info->key = key;
    outNode->info->value = value;
    outNode->childNodes = NULL;
    // if(jsonLinkedListCreate(&(outNode->childNodes)) != 1)
    //     return 0;
    int listFuncResult = jsonLinkedListCreate(&(outNode->childNodes));
    if(listFuncResult != 1)
        return 0;

    *out = outNode;
    return 1;
}
int jsonTreeFreeNode(JSONTreeNode **in, int freeChildNodes)
{
    if(in == NULL)
        return -1;
    
    JSONTreeNode *node = *in;

    // If it has children, loop through those first before freeing this node
    if(node->childNodes->size > 0)
    {
        int listAtFuncResult;
        int freeFuncResult;
        for (size_t i = 0; i < node->childNodes->size; i++)
        {
            JSONTreeNode *child = NULL;
            jsonLinkedListAtPtr(&(node->childNodes), i, &listAtFuncResult, child, JSONTreeNode*); 
            freeFuncResult = jsonTreeFreeNode(&child, freeChildNodes);
            if(freeFuncResult == 0)
                return 0;
        }
    }

    // Free the node components and the list of children
    // NOTE: Have some sort of way to relink or retrieve the child nodes if they are not to be freed would be dandy
    free(node->info->key);
    node->info->key = NULL;
    free(node->info->value.value);
    node->info->value.value = NULL;
    free(node->info);
    node->info = NULL;
    jsonLinkedListFree(&(node->childNodes), freeChildNodes);
    free(node);
    node = NULL;

    *in = node;
    return 1;
}

int jsonTreeInsert(JSONTree **treePtrPtr, JSONTreeNode **nodePtrPtr, JSONTreeNode **parentNodePtrPtr)
{
    if (treePtrPtr == NULL)
        return -1;

    if(nodePtrPtr == NULL)
        return -1;

    JSONTree *tree = *treePtrPtr;
    JSONTreeNode *node = *nodePtrPtr;

    if (tree == NULL)
        return -1;
    if (node == NULL)
        return 0;

    if (parentNodePtrPtr == NULL)
    {
        jsonLinkedListPushBack(&(tree->root->childNodes), (void *)node);
    }
    else
    {
        JSONTreeNode *parentNode = *parentNodePtrPtr;
        jsonLinkedListPushBack(&(parentNode->childNodes), (void *)node);
    }

    return 1;
}
int jsonTreeRemove(JSONTreeNode **nodePtrPtr, int freeChildren)
{
}

/*
Internal function for recursive looping through all of the children
of the provided start node and their children

Returns:
 1 -- node found
 0 -- node not found
-1 -- node not found, end of this branch

-1 signalizes the loop to move to the next branch of the tree
*/
static int _jsonFindNodeLoop(JSONTreeNode **start, const char* const key, JSONTreeNode **out)
{
    JSONTreeNode *startNode = *start;
    JSONTreeNode *currentNode = NULL;
    JSONTreeNode *desiredNode = NULL;

    int loopResult = 0;
    
    // Only attempt to loop through child nodes if there are any to begin with
    if(startNode->childNodes->size == 0)
    {
        // If this is the desired node, return it
        if(strcmp(startNode->info->key, key) == 0)
        {
            desiredNode = currentNode;
            loopResult = 1;
        }
        // If not, this is the end of the branch as no children are present
        else
            loopResult = -1;
    }
    else
    {
        // Go through each immediate child of the provided start node
        for (size_t i = 0; i < startNode->childNodes->size; i++)
        {
            int funcResult = 0;
            jsonLinkedListAtPtr(&startNode->childNodes, i, &funcResult, currentNode, JSONTreeNode*);
            // If this first level node is the desired node,
            // there's no need to loop through its children
            if(strcmp(currentNode->info->key, key) == 0)
            {
                desiredNode = currentNode;
                loopResult = 1;
                break;
            }
            // If it is not the desired node and if the node has children,
            // loop through each of the children and their children
            // recursively until either the desired node is found
            // or until all branches have been searched through
            else if(currentNode->childNodes->size > 0)
            {
                do
                {
                    loopResult = _jsonFindNodeLoop(&currentNode, key, &desiredNode);
                }
                while (loopResult == 0);

                // If the desired node is found,
                // break out of the first level children loop
                // because there isn't a point in looping through
                // all of them seeing as we already have our node
                if(loopResult == 1)
                    break;

                // If the entire subtree was explored, continue onto the next one
                if(loopResult == -1)
                    continue;
            }
            // Default: end of branch as there are no children present and this node wasn't the desired one
            else
            {
                loopResult = -1;
            }
        }
    }
    
    *out = desiredNode;
    return loopResult;
}
int jsonTreeGetNode(JSONTree **treePtrPtr, const char *const key, JSONTreeNode **out)
{
    // Check if all required args were provided with usable values
    if (treePtrPtr == NULL)
        return -1;

    if(out == NULL)
        return -1;

    if(strcmp(key, "") == 0)
        return 0;

    JSONTree *tree = *treePtrPtr;

    if(tree->root == NULL)
        return -1;
    if (tree->root->childNodes->size <= 0)
        return -1;

    /*
    Depth-First Search Preorder algorithm
    --------------------
    Starts at the root node and visits the entire first branch until moving on to the next branch
    When examining a branch, the algorithm first examines all nodes of said branch before searching through the next child branch

    Source: https://towardsdatascience.com/4-types-of-tree-traversal-algorithms-d56328450846
    --------------------
    Algorithm:
    1) Start at root
    2) For every child node of root (1st level), loop
    3) Check if child is not the desired node
    4) For every child node of the current node (2nd level), loop
    6) If end of branch, move onto the next branch (another 2nd level child)
    6) Repeat until node is found
    DO NOT MOVE ONTO EXAMINING OTHER CHILDREN BEFORE THE ENTIRE BRANCH IS CHECKED FIRST
    */

    JSONTreeNode *desiredNode = NULL;

    // Recursively search through the entire tree until 
    // either the node is found or until the end of the tree is reached
    int loopResult = 0;
    do
    {
        loopResult = _jsonFindNodeLoop(&tree->root, key, &desiredNode);
    }
    while(loopResult == 0);
    
    // Return the node
    *out = desiredNode;
    if (desiredNode == NULL)
        return 0;
    else
        return 1;
}