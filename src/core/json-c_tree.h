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
#ifndef JSON_C_TREE
#define JSON_C_TREE

#include "json-c_types.h"
#include "json-c_linked_list.h"

// NOTE: Get rid of JSONArray and JSONObject and simply use the JSONValueType
//       as a quick way of determining whether or not the tree search should proceed
//       to child nodes (if there are any), because Arrays and Objects are guaranteed 
//       to have values that need to be looped for, whereas the other types do not

typedef struct JSONTreeNode
{
    JSONPair *info;
    JSONLinkedList *childNodes;
} JSONTreeNode;

typedef struct JSONTree
{
    JSONLinkedList *nodes;
} JSONTree;

/*
Creates a new tree

If the provided tree ptr is not NULL, 
the function frees the currently present tree 
and replaces the old tree with an empty one

THE PASSED IN TREE PTR MUST BE SET TO NULL WHEN FIRST DECLARING THE TREE PTR VAR
like so:
JSONTree *tree = NULL;
jsonCreateTree(&tree);

Returns: 
1 -- successfully allocated a new empty tree
0 -- allocation was unsuccessful
*/
int jsonTreeCreate(JSONTree **treePtrPtr);
/*
Frees the provided tree and all of its nodes

If 'freeValues' parameter is set to 1, the values held by the tree's nodes will be freed as well

Returns: 
 1 -- all nodes were freed correctly
 0 -- some nodes were not freed
-1 -- provided tree is uninitialized
*/
int jsonTreeFree(JSONTree **treePtrPtr, int freeValues);

int jsonTreeCreateNode(JSONTreeNode **out, char* key, JSONValue value);

/*
Inserts a new node into the tree

If a parent node is specifies, the new node gets inserted as the parent node's child
Otherwise, it is inserted as a child of the root node

Returns: 
 1 -- node inserted successfully
 0 -- failed inserting node 
-1 -- provided tree is uninitialized
*/
int jsonTreeInsert(JSONTree **treePtrPtr, JSONTreeNode **nodePtrPtr, JSONTreeNode **parentNodePtrPtr);
/*
Removes the provided node

If 'freeChildren' parameter is set, all of the node's children get removed as well

Returns:
*/
int jsonTreeRemove(JSONTreeNode **nodePtrPtr, int freeChildren);

#endif