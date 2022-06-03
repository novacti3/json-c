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

// Because having to write this code in EVERY SINGLE TEST CASE is, frankly, quite tedious
#define CREATE_TREE(x) \
JSONTree* x = NULL; \
jsonTreeCreate(&x); \

// Set of constants to test against in all test cases
static JSONTreeNode* NODE_ONE;
static JSONTreeNode* NODE_TWO;
static JSONTreeNode* NODE_THREE; 

// Forward declarations of methods
// that will test the functionality
// of each aspect of the linked lists

void TestTreeCreate(CuTest *test);
void TestTreeInsert(CuTest *test);
void TestTreeFree(CuTest *test);

// MAIN FUNC
int main()
{
    CuSuite *suite = CuSuiteNew();
    CuSuiteInit(suite);

    // Create the tests
    CuTest *testTreeCreate = CuTestNew("Create tree", &TestTreeCreate);
    CuTest *testTreeInsert = CuTestNew("Insert node into tree", &TestTreeInsert);
    CuTest *testTreeFree = CuTestNew("Free tree", &TestTreeFree);

    // Add tests to suite
    CuSuiteAdd(suite, testTreeCreate);
    CuSuiteAdd(suite, testTreeInsert);
    CuSuiteAdd(suite, testTreeFree);

    // Initialize constants
    jsonTreeCreateNode(&NODE_ONE, "node_one", (JSONValue){.value = (void*)5, .type = JSON_VALUE_TYPE_INT});
    jsonTreeCreateNode(&NODE_TWO, "node_two", (JSONValue){.value = (void*)-4, .type = JSON_VALUE_TYPE_INT});
    jsonTreeCreateNode(&NODE_THREE, "node_three", (JSONValue){.value = NULL, .type = JSON_VALUE_TYPE_NULL});

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
    // Assert if the start of the list not empty
    CuAssertPtrNotNull(test, tree->nodes);
}

void TestTreeInsert(CuTest *test)
{
    CREATE_TREE(tree);

    jsonTreeInsert(&tree, &NODE_ONE, NULL);
    CuAssertIntEquals(test, 1, tree->nodes->size);

    jsonTreeInsert(&tree, &NODE_TWO, NULL);
    CuAssertIntEquals(test, 2, tree->nodes->size);

    jsonTreeInsert(&tree, &NODE_THREE, &NODE_ONE);
    CuAssertIntEquals(test, 2, tree->nodes->size);
    CuAssertIntEquals(test, 1, NODE_ONE->childNodes->size);
}

void TestTreeFree(CuTest *test)
{
    // TODO: Tree free test
}