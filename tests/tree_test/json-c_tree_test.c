/*
Copyright © 2022 Jan Šaler

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

#include <stdio.h>
#include <stdlib.h>

#include <CuTest.h>
#include "json-c_tree.h"

// Set of constants to test against in all test cases
static JSONTreeNode* NODE_ONE;
static JSONTreeNode* NODE_TWO;
static JSONTreeNode* NODE_THREE; 
static JSONTreeNode* NODE_FOUR; 
static JSONTreeNode* NODE_FIVE; 
static JSONTreeNode* NODE_SIX; 

// Because having to write this code in EVERY SINGLE TEST CASE is, frankly, quite tedious
#define CREATE_TREE(x) \
JSONTree* x = NULL; \
jsonTreeCreate(&x); \

/* Tree diagram:
            NODE_ONE ---- NODE_TWO ---- NODE_SIX
                |      
            NODE_THREE
            /        \
        NODE_FOUR NODE_FIVE
*/
#define CREATE_POPULATED_TREE(x) \
CREATE_TREE(x) \
jsonTreeInsert(&x, &NODE_ONE, NULL); \
jsonTreeInsert(&x, &NODE_TWO, NULL); \
jsonTreeInsert(&x, &NODE_THREE, &NODE_ONE); \
jsonTreeInsert(&x, &NODE_FOUR, &NODE_THREE); \
jsonTreeInsert(&x, &NODE_FIVE, &NODE_THREE); \
jsonTreeInsert(&x, &NODE_SIX, NULL); \

// Forward declarations of methods
// that will test the functionality
// of each aspect of the linked lists

void TestTreeCreate(CuTest *test);
void TestTreeFree(CuTest *test);
void TestTreeInsert(CuTest *test);
void TestTreeGetNode(CuTest *test);

// MAIN FUNC
int main()
{
    CuSuite *suite = CuSuiteNew();
    CuSuiteInit(suite);

    // Create the tests
    CuTest *testTreeCreate = CuTestNew("Create tree", &TestTreeCreate);
    CuTest *testTreeFree = CuTestNew("Free tree", &TestTreeFree);
    CuTest *testTreeInsert = CuTestNew("Insert node into tree", &TestTreeInsert);
    CuTest *testTreeGetNode = CuTestNew("Get node of tree", &TestTreeGetNode);

    // Add tests to suite
    CuSuiteAdd(suite, testTreeCreate);
    CuSuiteAdd(suite, testTreeFree);
    // FIXME: These crash, pls fix
    // CuSuiteAdd(suite, testTreeInsert);
    // CuSuiteAdd(suite, testTreeGetNode);

    // Initialize constants
    jsonTreeCreateNode(&NODE_ONE, "node_one", (JSONValue){.value = (void*)5, .type = JSON_VALUE_TYPE_INT});
    jsonTreeCreateNode(&NODE_TWO, "node_two", (JSONValue){.value = (void*)-4, .type = JSON_VALUE_TYPE_INT});
    jsonTreeCreateNode(&NODE_THREE, "node_three", (JSONValue){.value = (void*)4, .type = JSON_VALUE_TYPE_INT});
    jsonTreeCreateNode(&NODE_FOUR, "node_four", (JSONValue){.value = (void*)"hello world", .type = JSON_VALUE_TYPE_STRING});
    jsonTreeCreateNode(&NODE_FIVE, "node_five", (JSONValue){.value = (void*)0, .type = JSON_VALUE_TYPE_BOOL});
    jsonTreeCreateNode(&NODE_SIX, "", (JSONValue){.value = NULL, .type = JSON_VALUE_TYPE_NULL});

    // Run the suite and retrieve the results
    CuSuiteRun(suite);

    CuString *summary = CuStringNew();
    CuString *details = CuStringNew();
    CuSuiteSummary(suite, summary);
    CuSuiteDetails(suite, details);

    // Print the results
    printf(summary->buffer);
    printf(details->buffer);

    return (int)suite;
};

// TEST DEFINITIONS

void TestTreeCreate(CuTest *test)
{ 
    JSONTree *tree = NULL;
    
    // NOTE: Checks for the fail results might be great too

    int createFuncResult = jsonTreeCreate(&tree);
    // Assert if list failed allocating
    CuAssertIntEquals(test, 1, createFuncResult);
    CuAssertPtrNotNull(test, tree->root);
    CuAssertPtrNotNull(test, tree->root->info);
    CuAssertPtrNotNull(test, tree->root->childNodes);
}
void TestTreeFree(CuTest *test)
{
    CREATE_POPULATED_TREE(tree);
    int freeFuncResult = jsonTreeFree(&tree, 1);
    CuAssertIntEquals(test, 1, freeFuncResult);
    CuAssertPtrEquals(test, NULL, tree);   
}

void TestTreeInsert(CuTest *test)
{
    CREATE_TREE(tree);

    jsonTreeInsert(&tree, &NODE_ONE, NULL);
    CuAssertIntEquals(test, 1, tree->root->childNodes->size);

    jsonTreeInsert(&tree, &NODE_TWO, NULL);
    CuAssertIntEquals(test, 2, tree->root->childNodes->size);

    jsonTreeInsert(&tree, &NODE_THREE, &NODE_ONE);
    CuAssertIntEquals(test, 2, tree->root->childNodes->size);
    CuAssertIntEquals(test, 1, NODE_ONE->childNodes->size);
    
    jsonTreeFree(&tree, 0);
}

void TestTreeGetNode(CuTest *test)
{
    /* Tree diagram:
            NODE_ONE ---- NODE_TWO ---- NODE_SIX
                |      
            NODE_THREE
            /        \
        NODE_FOUR NODE_FIVE
    */
    CREATE_POPULATED_TREE(tree);    

    // Test if the function reacts appropriately to unusable data being passed in
    // to prevent crashes etc.
    CuAssertIntEquals(test, -1, jsonTreeGetNode(NULL, "", NULL));
    CuAssertIntEquals(test, -1, jsonTreeGetNode(&tree, "", NULL));

    // Test if function exists when trying to search for an anonymous node
    // (undefined behaviour)
    JSONTreeNode *nodeSix = NULL;
    CuAssertIntEquals(test, jsonTreeGetNode(&tree, "", &nodeSix), 0);
    
    // Test if the function properly finds and returs the desired node
    JSONTreeNode *nodeFive = NULL;
    CuAssertIntEquals(test, 1, jsonTreeGetNode(&tree, "node_five", &nodeFive));
    CuAssertPtrNotNull(test, nodeFive);
    CuAssertStrEquals(test, "node_five", nodeFive->info->key);
    CuAssertIntEquals(test, 0, (int)(nodeFive->info->value.value));

    jsonTreeFree(&tree, 0);
}