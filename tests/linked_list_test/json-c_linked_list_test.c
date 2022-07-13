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
void TestListRemove(CuTest *test);
void TestListRemoveAt(CuTest *test);

void TestListAt(CuTest *test);
void TestListContains(CuTest *test);

void TestListPushFront(CuTest *test);
void TestListPopFront(CuTest *test);

void TestListPushBack(CuTest *test);
void TestListPopBack(CuTest *test);

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
    CuTest *testListRemove = CuTestNew("Remove from list", &TestListRemove);
    CuTest *testListRemoveAt = CuTestNew("Remove from list at index", &TestListRemoveAt);
    
    CuTest *testListAt = CuTestNew("Get element at index", &TestListAt);
    CuTest *testListContains = CuTestNew("List contains value", &TestListContains);
    
    CuTest *testListPushFront = CuTestNew("Push front", &TestListPushFront);
    CuTest *testListPopFront = CuTestNew("Pop front", &TestListPopFront);

    CuTest *testListPushBack = CuTestNew("Push back", &TestListPushBack);
    CuTest *testListPopBack = CuTestNew("Pop back", &TestListPopBack);
    
    CuTest *testListToArray = CuTestNew("Convert list to array", &TestListToArray);

    // Add tests to suite
    CuSuiteAdd(suite, testListCreate);
    CuSuiteAdd(suite, testListFree);

    CuSuiteAdd(suite, testListInsert);
    CuSuiteAdd(suite, testListRemove);
    CuSuiteAdd(suite, testListRemoveAt);

    CuSuiteAdd(suite, testListAt);
    CuSuiteAdd(suite, testListContains);
    
    CuSuiteAdd(suite, testListPushFront);
    CuSuiteAdd(suite, testListPopFront);

    CuSuiteAdd(suite, testListPushBack);
    CuSuiteAdd(suite, testListPopBack);

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

    // NOTE: Checks for the fail results might be great too

    // Insert first value
    jsonLinkedListInsert(&list, 0, &VALUE_ONE);
    // Check if the size properly reflects the amount of nodes in the list
    CuAssertIntEquals(test, 1, list->size);
    // Check if the node got properly assigned as the start of the list
    CuAssertPtrNotNull(test, list->start);
    
    // Value check
    int firstValue;
    jsonLinkedListAt(&list, 0, firstValue, int);
    // Check if the retrieved value corresponds to the value that was supposed to be inserted
    CuAssertIntEquals(test, VALUE_ONE, firstValue);

    // Attempt to insert second value
    // This should fail because the index is more than the list's size - 1
    jsonLinkedListInsert(&list, 1, &VALUE_TWO);
    // Check if the size properly reflects the amount of nodes in the list
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrEquals(test, NULL, list->start->next);
    
    // Push back the second value to the list so that it can be replaced later
    jsonLinkedListPushBack(&list, &VALUE_TWO);
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start->next);

    // Replace second value with other value
    jsonLinkedListInsert(&list, 1, &VALUE_THREE);
    // Check that the size didn't change because no new node was supposed to be appended to the list
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start->next);
    
    // Value check
    int thirdValue;
    jsonLinkedListAt(&list, 1, thirdValue, int);
    // Check that the retrieved value corresponds to the value that was supposed to replace the previous value
    CuAssertIntEquals(test, VALUE_THREE, thirdValue);
    // Check that no node was added to the list
    CuAssertPtrEquals(test, NULL, list->start->next->next);
}
void TestListRemove(CuTest *test)
{
    CREATE_LIST(list);

    // Insert several values to the list so that something can actually be removed
    jsonLinkedListPushBack(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);

    jsonLinkedListRemove(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 0, list->size);
    CuAssertPtrEquals(test, NULL, list->start);

    jsonLinkedListPushBack(&list, &VALUE_TWO);
    jsonLinkedListPushBack(&list, &VALUE_THREE);
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrNotNull(test, list->start->next);
    CuAssertPtrEquals(test, NULL, list->start->next->next);

    jsonLinkedListRemove(&list, &VALUE_THREE);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrEquals(test, NULL, list->start->next);
}
void TestListRemoveAt(CuTest *test)
{
    CREATE_LIST(list);

    // Insert several values to the list so that something can actually be removed
    jsonLinkedListPushBack(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);

    jsonLinkedListRemoveAt(&list, 0);
    CuAssertIntEquals(test, 0, list->size);
    CuAssertPtrEquals(test, NULL, list->start);

    jsonLinkedListPushBack(&list, &VALUE_TWO);
    jsonLinkedListPushBack(&list, &VALUE_THREE);
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrNotNull(test, list->start->next);
    CuAssertPtrEquals(test, NULL, list->start->next->next);

    jsonLinkedListRemoveAt(&list, 1);
    CuAssertIntEquals(test, 1, list->size);
    CuAssertPtrNotNull(test, list->start);
    CuAssertPtrEquals(test, NULL, list->start->next);
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
void TestListContains(CuTest *test)
{
    CREATE_LIST(list);

    jsonLinkedListPushFront(&list, &VALUE_ONE);
    CuAssertIntEquals(test, 1, jsonLinkedListContains(&list, &VALUE_ONE));

    jsonLinkedListPushFront(&list, &VALUE_TWO);
    CuAssertIntEquals(test, 1, jsonLinkedListContains(&list, &VALUE_TWO));
    CuAssertIntEquals(test, 0, jsonLinkedListContains(&list, &VALUE_THREE));
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