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

int jsonTreeCreate(JSONTree **treePtrPtr);
int jsonTreeFree(JSONTree **treePtrPtr, int freeValues);

#endif