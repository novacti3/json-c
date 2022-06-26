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

// Forward declarations of methods
// that will test the functionality
// of each aspect of the tree
void TestTreeCreate(CuTest *test);
void TestTreeFree(CuTest *test);
void TestTreeInsert(CuTest *test);
void TestTreeGetNode(CuTest *test);

// Helper function for initiating the tree nodes to insert into the test tree
// Required because nodes have to be reinitialized after every test due to freeing
// the old tree to prevent nodes having children with corrupt data, given that they are static
static void InitTreeNodes()
{
    jsonTreeCreateNode(&NODE_ONE, "node_one", (JSONValue){.value = (void*)5, .type = JSON_VALUE_TYPE_INT}); 
    jsonTreeCreateNode(&NODE_TWO, "node_two", (JSONValue){.value = (void*)-4, .type = JSON_VALUE_TYPE_INT}); 
    jsonTreeCreateNode(&NODE_THREE, "node_three", (JSONValue){.value = (void*)4, .type = JSON_VALUE_TYPE_INT}); 
    jsonTreeCreateNode(&NODE_FOUR, "node_four", (JSONValue){.value = (void*)"hello world", .type = JSON_VALUE_TYPE_STRING}); 
    jsonTreeCreateNode(&NODE_FIVE, "node_five", (JSONValue){.value = (void*)0, .type = JSON_VALUE_TYPE_BOOL}); 
    jsonTreeCreateNode(&NODE_SIX, "", (JSONValue){.value = NULL, .type = JSON_VALUE_TYPE_NULL});
}
// Helper function to create a tree populated with nodes following this diagram:
/* Tree diagram:
            NODE_ONE ---- NODE_TWO ---- NODE_SIX
                |      
            NODE_THREE
            /        \
        NODE_FOUR NODE_FIVE
*/
static void InitPopulatedTree(JSONTree **treePtrPtr)
{
    JSONTree *tree = NULL;
    jsonTreeCreate(&tree);
    InitTreeNodes(); 

    jsonTreeInsert(&tree, &NODE_ONE, NULL); 
    jsonTreeInsert(&tree, &NODE_TWO, NULL); 
    jsonTreeInsert(&tree, &NODE_THREE, &NODE_ONE); 
    jsonTreeInsert(&tree, &NODE_FOUR, &NODE_THREE); 
    jsonTreeInsert(&tree, &NODE_FIVE, &NODE_THREE); 
    jsonTreeInsert(&tree, &NODE_SIX, NULL);
    
    *treePtrPtr = tree;
}

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
    CuSuiteAdd(suite, testTreeInsert);
    CuSuiteAdd(suite, testTreeGetNode);

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
    // Successfully created the tree
    CuAssertIntEquals(test, 1, createFuncResult);
    // Successfully allocated memory for the root node of the tree
    CuAssertPtrNotNull(test, tree->root);
    // Successfully allocated memory for the info struct of the root node
    CuAssertPtrNotNull(test, tree->root->info);
    // Successfully allocated memory for the list of child nodes of the root node
    CuAssertPtrNotNull(test, tree->root->childNodes);
}
void TestTreeFree(CuTest *test)
{
    JSONTree *tree = NULL;
    InitPopulatedTree(&tree);
    
    int freeFuncResult = jsonTreeFree(&tree, 1);
    // Successfully freed the tree
    CuAssertIntEquals(test, 1, freeFuncResult);
    // The tree ptr is null
    CuAssertPtrEquals(test, NULL, tree);   
}

void TestTreeInsert(CuTest *test)
{
    JSONTree *tree = NULL;
    jsonTreeCreate(&tree);
    InitTreeNodes();

    jsonTreeInsert(&tree, &NODE_ONE, NULL);
    // The list of root's children has successfully increased by one upon insertion
    CuAssertIntEquals(test, 1, tree->root->childNodes->size);

    jsonTreeInsert(&tree, &NODE_TWO, NULL);
    // The list of root's children has successfully increased by one upon insertion
    CuAssertIntEquals(test, 2, tree->root->childNodes->size);

    jsonTreeInsert(&tree, &NODE_THREE, &NODE_ONE);
    // The list of root's children has stayed the same upon insertion
    // due to the parent node parameter being provided
    CuAssertIntEquals(test, 2, tree->root->childNodes->size);
    // The list of the parent node's children has increased by one
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
    JSONTree *tree = NULL;
    InitPopulatedTree(&tree);    

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