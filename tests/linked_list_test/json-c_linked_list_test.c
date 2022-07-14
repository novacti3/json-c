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

// TODO: Add a comment for each CuAssert explaining what it is checking for
// TODO: REFACTOR THESE and make sure that even the func result and invalid params get checked for

#include <stdio.h>
#include <stdlib.h>

#include <CuTest.h>
#include "json-c_linked_list.h"

// Because having to write this code in EVERY SINGLE TEST CASE is, frankly, quite tedious
#define CREATE_LIST(x) \
JSONLinkedList* x = NULL; \
jsonLinkedListCreate(&x); \

// Set of constants to test against in all test cases
static int VALUE_ONE = 5;
static int VALUE_TWO = -4;
static int VALUE_THREE = 0;


// Forward declarations of methods
// that will test the functionality
// of each aspect of the linked lists

void TestListCreate(CuTest *test);
void TestListFree(CuTest *test);

void TestListInsert(CuTest *test);
void TestListReplace(CuTest *test);
void TestListRemove(CuTest *test);
void TestListRemoveAt(CuTest *test);

void TestListPushFront(CuTest *test);
void TestListPopFront(CuTest *test);

void TestListPushBack(CuTest *test);
void TestListPopBack(CuTest *test);

void TestListContains(CuTest *test);

void TestListAt(CuTest *test);

void TestListToArray(CuTest *test);

// MAIN FUNC
int main()
{
    CuSuite *suite = CuSuiteNew();
    CuSuiteInit(suite);

    // Create the tests
    CuTest *testListCreate = CuTestNew("Create list", &TestListCreate);
    CuTest *testListFree = CuTestNew("Free List", &TestListFree);
    
    CuTest *testListInsert = CuTestNew("Insert into list", &TestListInsert);
    CuTest *testListReplace = CuTestNew("Replace element in list", &TestListReplace);
    CuTest *testListRemove = CuTestNew("Remove from list", &TestListRemove);
    CuTest *testListRemoveAt = CuTestNew("Remove from list at index", &TestListRemoveAt);
    
    CuTest *testListPushFront = CuTestNew("Push front", &TestListPushFront);
    CuTest *testListPopFront = CuTestNew("Pop front", &TestListPopFront);

    CuTest *testListPushBack = CuTestNew("Push back", &TestListPushBack);
    CuTest *testListPopBack = CuTestNew("Pop back", &TestListPopBack);
    
    CuTest *testListContains = CuTestNew("List contains value", &TestListContains);
    
    CuTest *testListAt = CuTestNew("Get element at index", &TestListAt);
    
    CuTest *testListToArray = CuTestNew("Convert list to array", &TestListToArray);

    // Add tests to suite
    CuSuiteAdd(suite, testListCreate);
    CuSuiteAdd(suite, testListFree);

    CuSuiteAdd(suite, testListInsert);
    CuSuiteAdd(suite, testListReplace);
    CuSuiteAdd(suite, testListRemove);
    CuSuiteAdd(suite, testListRemoveAt);

    CuSuiteAdd(suite, testListPushFront);
    CuSuiteAdd(suite, testListPopFront);

    CuSuiteAdd(suite, testListPushBack);
    CuSuiteAdd(suite, testListPopBack);

    CuSuiteAdd(suite, testListContains);
    
    CuSuiteAdd(suite, testListAt);
    
    CuSuiteAdd(suite, testListToArray);

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

void TestListCreate(CuTest *test)
{ 
    JSONLinkedList *list;
    int funcResult = 0;    

    // Invalid input param check
    funcResult = jsonLinkedListCreate(NULL);
    CuAssertIntEquals(test, -1, funcResult);
    
    // Check if creation was successful
    funcResult = jsonLinkedListCreate(&list);
    CuAssertIntEquals(test, 1, funcResult);
    // Assert if the start of the list not empty
    CuAssertPtrEquals(test, NULL, list->start);
}
void TestListFree(CuTest *test)
{
    // Checks for invalid list ptr provided
    JSONLinkedList *invalidList;
    int funcResult = 0;
    funcResult = jsonLinkedListFree(NULL, 0);
    // No further checks for funcResult are required 
    // as the func is guaranteed to succeed every time
    CuAssertIntEquals(test, -1, funcResult);
    
    // Make sure that the func returns invalid param code
    // if the pointed to ptr is NULL as well
    invalidList = NULL;
    funcResult = jsonLinkedListFree(&invalidList, 0);
    CuAssertIntEquals(test, -1, funcResult);
    
    // Create list
    JSONLinkedList *list;
    jsonLinkedListCreate(&list);
    // Ensure that the list ptr itself gets freed even if it has no nodes
    jsonLinkedListFree(&list, 1);
    CuAssertPtrEquals(test, NULL, list);

    // Populate a newly created list
    jsonLinkedListCreate(&list);
    jsonLinkedListPushBack(&list, &VALUE_ONE);
    jsonLinkedListPushBack(&list, &VALUE_TWO);
    jsonLinkedListPushBack(&list, &VALUE_THREE);
    // Make sure that if freeValues is 0, the values really don't get freed
    jsonLinkedListFree(&list, 0);
    CuAssertPtrEquals(test, NULL, list);

    // Populate a newly created list
    jsonLinkedListCreate(&list);
    jsonLinkedListPushBack(&list, &VALUE_ONE);
    jsonLinkedListPushBack(&list, &VALUE_TWO);
    jsonLinkedListPushBack(&list, &VALUE_THREE);
    // Ensure that everything proceeds when 'freeValues' is specified too
    jsonLinkedListFree(&list, 1);
    CuAssertPtrEquals(test, NULL, list);
    // NOTE: Checking whether or not the data itself was actually freed
    //       along with the nodes is not possible as jsonLinkedListPushBack
    //       copies said data. By the time jsonLinkedListFree finishes,
    //       everything is freed already
}

void TestListInsert(CuTest *test)
{
    CREATE_LIST(list);
    int funcResult = 0;

    // Invalid param checks
    funcResult = jsonLinkedListInsert(NULL, 0, &VALUE_ONE);
    CuAssertIntEquals(test, -1, funcResult);
    funcResult = jsonLinkedListInsert(&list, 0, NULL);
    CuAssertIntEquals(test, -1, funcResult);

    // Index out of range checks
    funcResult = jsonLinkedListInsert(&list, -2, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);
    funcResult = jsonLinkedListInsert(&list, 2, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);

    // Insert first value
    funcResult = jsonLinkedListInsert(&list, 0, &VALUE_ONE);
    CuAssertIntEquals(test, 1, funcResult);
    // Check that size properly reflects the amount of nodes in the list
    CuAssertIntEquals(test, 1, list->size);
    // Ensure that the new node got properly assigned as the start of the list
    CuAssertPtrNotNull(test, list->start);
    // Check if the inserted value matches the one that was supposed to be inserted
    CuAssertIntEquals(test, VALUE_ONE, *((int*)(list->start->data)));

    // Check that inserting an element to the next index in a list works fine
    funcResult = jsonLinkedListInsert(&list, 1, &VALUE_TWO);
    CuAssertIntEquals(test, 1, funcResult);
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start->next);

    // Replace second value with other value
    funcResult = jsonLinkedListInsert(&list, 1, &VALUE_THREE);
    CuAssertIntEquals(test, 1, funcResult);
    // Check that the size didn't change because no new node was supposed to be appended to the list
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrEquals(test, NULL, list->start->next->next);
    // Make sure that the node itself wasn't freed rather than just the data
    CuAssertPtrNotNull(test, list->start->next);
    // Check that the retrieved value corresponds to the value that replaced the previous value
    CuAssertIntEquals(test, VALUE_THREE, *((int*)(list->start->next->data)));
}
void TestListReplace(CuTest *test)
{
    CREATE_LIST(list);
    int funcResult = 0;

    // Invalid param checks
    funcResult = jsonLinkedListReplace(NULL, 0, &VALUE_ONE);
    CuAssertIntEquals(test, -1, funcResult);
    funcResult = jsonLinkedListReplace(&list, 0, NULL);
    CuAssertIntEquals(test, -1, funcResult);

    // Index out of range checks
    funcResult = jsonLinkedListReplace(&list, -2, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);
    funcResult = jsonLinkedListReplace(&list, 2, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);

    // Make sure that replacing the start node before it was initialized
    // doesn't proceed as that could lead to a segfault
    funcResult = jsonLinkedListReplace(&list, 0, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);

    // Insert first value
    jsonLinkedListInsert(&list, 0, &VALUE_ONE);
    // Replace the start node
    funcResult = jsonLinkedListReplace(&list, 0, &VALUE_TWO);
    CuAssertIntEquals(test, 1, funcResult);
    CuAssertIntEquals(test, VALUE_TWO, *((int*)(list->start->data)));

    // Insert another value
    jsonLinkedListInsert(&list, 1, &VALUE_THREE);
    // Replace the new value with a different one
    funcResult = jsonLinkedListReplace(&list, 1, &VALUE_TWO);
    CuAssertIntEquals(test, 1, funcResult);
    CuAssertIntEquals(test, VALUE_TWO, *((int*)(list->start->next->data)));

}
void TestListRemove(CuTest *test)
{
    CREATE_LIST(list);
    int funcResult = 0;

    // Invalid param checks
    funcResult = jsonLinkedListRemove(NULL, &VALUE_ONE);
    CuAssertIntEquals(test, -1, funcResult);
    funcResult = jsonLinkedListRemove(&list, NULL);
    CuAssertIntEquals(test, -1, funcResult);
    
    // Make sure that an empty list doesn't crash something
    funcResult = jsonLinkedListRemove(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);

    // Insert a value to the list so that there is something to remove
    jsonLinkedListInsert(&list, 0, &VALUE_ONE);
    // Remove said value
    funcResult = jsonLinkedListRemove(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 1, funcResult);
    CuAssertIntEquals(test, 0, list->size);
    CuAssertPtrEquals(test, NULL, list->start);

    // Insert more stuff into the list
    jsonLinkedListInsert(&list, 0, &VALUE_TWO);
    jsonLinkedListInsert(&list, 1, &VALUE_THREE);
    // See if searching for data not present in the list returns 0 as it should
    funcResult = jsonLinkedListRemove(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);
    // Test if removing data in a list with more elements works fine
    funcResult = jsonLinkedListRemove(&list, &VALUE_THREE);
    CuAssertIntEquals(test, 1, funcResult);
}
void TestListRemoveAt(CuTest *test)
{
    CREATE_LIST(list);
    int funcResult = 0;

    // Invalid ptr check
    funcResult = jsonLinkedListRemoveAt(NULL, 0);
    CuAssertIntEquals(test, -1, funcResult);
    // Can't remove something from an empty list check
    funcResult = jsonLinkedListRemoveAt(&list, 0);
    CuAssertIntEquals(test, 0, funcResult);

    // Insert something into the list so that there is something to remove
    jsonLinkedListInsert(&list, 0, &VALUE_ONE);
    // Remove said thing
    funcResult = jsonLinkedListRemoveAt(&list, 0);
    CuAssertIntEquals(test, 1, funcResult);
    CuAssertIntEquals(test, 0, list->size);
    CuAssertPtrEquals(test, NULL, list->start);

    // Make sure that removal goes smoothly even in lists with more elements
    jsonLinkedListInsert(&list, 0, &VALUE_TWO);
    jsonLinkedListInsert(&list, 1, &VALUE_THREE);
    funcResult = jsonLinkedListRemoveAt(&list, 1);
    CuAssertIntEquals(test, 1, funcResult);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrEquals(test, NULL, list->start->next);

    // Make sure that removing the start node works
    jsonLinkedListInsert(&list, 1, &VALUE_THREE);
    funcResult = jsonLinkedListRemoveAt(&list, 0);
    CuAssertIntEquals(test, 1, funcResult);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrEquals(test, NULL, list->start->next);
}

void TestListPushFront(CuTest *test)
{
    CREATE_LIST(list);

    jsonLinkedListPushFront(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrEquals(test, NULL, list->start->next);

    jsonLinkedListPushFront(&list, &VALUE_TWO);
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrNotNull(test, list->start->next);
}
void TestListPopFront(CuTest *test)
{
    CREATE_LIST(list);

    jsonLinkedListPushFront(&list, &VALUE_ONE);
    jsonLinkedListPushFront(&list, &VALUE_TWO);
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrNotNull(test, list->start->next);

    jsonLinkedListPopFront(&list);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrEquals(test, NULL, list->start->next);
}

void TestListPushBack(CuTest *test)
{
    CREATE_LIST(list);

    jsonLinkedListPushBack(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);

    jsonLinkedListPushBack(&list, &VALUE_TWO);
    CuAssertIntEquals(test, 2, list->size);

    jsonLinkedListPushBack(&list, &VALUE_THREE);
    CuAssertIntEquals(test, 3, list->size);

    int val;
    jsonLinkedListAt(&list, 2, val, int);
}
void TestListPopBack(CuTest *test)
{
    CREATE_LIST(list);

    jsonLinkedListPushBack(&list, &VALUE_ONE);
    jsonLinkedListPushBack(&list, &VALUE_TWO);
    jsonLinkedListPushBack(&list, &VALUE_THREE);
    CuAssertIntEquals(test, 3, list->size);

    jsonLinkedListPopBack(&list);
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrEquals(test, NULL, list->start->next->next);
}

void TestListContains(CuTest *test)
{
    CREATE_LIST(list);
    int funcResult = 0;

    // Invalid param checks
    funcResult = jsonLinkedListContains(NULL, &VALUE_ONE);
    CuAssertIntEquals(test, -1, funcResult);
    funcResult = jsonLinkedListContains(&list, NULL);
    CuAssertIntEquals(test, -1, funcResult);

    // Make sure that attempting to loop through an empty list returns 0
    funcResult = jsonLinkedListContains(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 0, funcResult);

    jsonLinkedListPushFront(&list, &VALUE_ONE);
    // Check if the value is successfully found in a list of only 1 element
    funcResult = jsonLinkedListContains(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 1, funcResult);

    jsonLinkedListPushFront(&list, &VALUE_TWO);
    // Check if value is found in a list of more elements
    funcResult = jsonLinkedListContains(&list, &VALUE_TWO);
    CuAssertIntEquals(test, 1, funcResult);
    // Make sure that a value not present in the list
    // returns the appropriate code
    funcResult = jsonLinkedListContains(&list, &VALUE_THREE);
    CuAssertIntEquals(test, 0, funcResult);
}

void TestListAt(CuTest *test)
{
    CREATE_LIST(list);

    // NOTE: Checks for the fail results might be great too

    // Insert several values to the list so that something can actually be retrieved from it
    jsonLinkedListPushBack(&list, &VALUE_ONE);
    jsonLinkedListPushBack(&list, &VALUE_TWO);
    jsonLinkedListPushBack(&list, &VALUE_THREE);

    // Checks that all nodes got appended correctly
    CuAssertIntEquals(test, 3, list->size);
    CuAssertPtrEquals(test, NULL, list->start->next->next->next);

    // Retrieve values
    int val1, val2, val3;
    jsonLinkedListAt(&list, 0, val1, int);
    jsonLinkedListAt(&list, 1, val2, int);
    jsonLinkedListAt(&list, 2, val3, int);
    // Check that the value at the index is what was inserted
    CuAssertIntEquals(test, VALUE_ONE, val1);
    CuAssertIntEquals(test, VALUE_TWO, val2);
    CuAssertIntEquals(test, VALUE_THREE, val3);

    int funcResult1;
    int* val1Ptr;
    jsonLinkedListAtPtr(&list, 0, &funcResult1, val1Ptr, int*);
    CuAssertPtrEquals(test, &VALUE_ONE, val1Ptr);
}

void TestListToArray(CuTest *test)
{
    CREATE_LIST(list);

    jsonLinkedListPushBack(&list, &VALUE_ONE);
    jsonLinkedListPushBack(&list, &VALUE_TWO);
    jsonLinkedListPushBack(&list, &VALUE_THREE);

    int **array = NULL;
    jsonLinkedListToArray(&list, array, int);

    int values[] = 
    { 
        *(array[0]), 
        *(array[1]), 
        *(array[2])
    };

    CuAssertIntEquals(test, VALUE_ONE, values[0]);
    CuAssertIntEquals(test, VALUE_TWO, values[1]);
    CuAssertIntEquals(test, VALUE_THREE, values[2]);
}