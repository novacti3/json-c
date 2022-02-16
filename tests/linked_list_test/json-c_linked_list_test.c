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
#include "json-c_linked_list.h"

// Set of constants to test against in all test cases

static int VALUE_ONE = 5;
static int VALUE_TWO = -4;
static int VALUE_THREE = 0;


// Forward declarations of methods
// that will test the functionality
// of each aspect of the linked lists

void TestListCreate(CuTest *test);

void TestListInsert(CuTest *test);
void TestListRemove(CuTest *test);

void TestListAt(CuTest *test);
void TestListContains(CuTest *test);

void TestListPushFront(CuTest *test);
void TestListPopFront(CuTest *test);

void TestListPushBack(CuTest *test);
void TestListPopBack(CuTest *test);

void TestListToArray(CuTest *test);

void TestListFree(CuTest *test);

int main()
{
    CuSuite *suite = CuSuiteNew();
    CuSuiteInit(suite);

    // Create the tests
    CuTest *testListCreate = CuTestNew("Create list", &TestListCreate);
    
    CuTest *testListInsert = CuTestNew("Insert into list", &TestListInsert);
    CuTest *testListRemove = CuTestNew("Remove from list", &TestListRemove);
    
    CuTest *testListAt = CuTestNew("Get element at index", &TestListAt);
    CuTest *testListContains = CuTestNew("List contains value", &TestListContains);
    
    CuTest *testListPushFront = CuTestNew("Push front", &TestListPushFront);
    CuTest *testListPopFront = CuTestNew("Pop front", &TestListPopFront);

    CuTest *testListPushBack = CuTestNew("Push back", &TestListPushBack);
    CuTest *testListPopBack = CuTestNew("Pop back", &TestListPopBack);
    
    CuTest *testListToArray = CuTestNew("Convert list to array", &TestListToArray);

    CuTest *testListFree = CuTestNew("Free List", &TestListFree);

    // Add tests to suite
    CuSuiteAdd(suite, testListCreate);

    CuSuiteAdd(suite, testListInsert);
    CuSuiteAdd(suite, testListRemove);

    CuSuiteAdd(suite, testListAt);
    CuSuiteAdd(suite, testListContains);
    
    CuSuiteAdd(suite, testListPushFront);
    CuSuiteAdd(suite, testListPopFront);

    CuSuiteAdd(suite, testListPushBack);
    CuSuiteAdd(suite, testListPopBack);

    CuSuiteAdd(suite, testListToArray);

    CuSuiteAdd(suite, testListFree);

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

void TestListCreate(CuTest *test)
{ 
    JSONLinkedList *list = NULL;
    
    int createFuncResult = jsonLinkedListCreate(&list);
    // Assert if list failed allocating
    CuAssertIntEquals(test, 1, createFuncResult);
    // Assert if the start of the list not empty
    CuAssertPtrEquals(test, NULL, list->start);
}

void TestListInsert(CuTest *test)
{
    JSONLinkedList *list = NULL;
    jsonLinkedListCreate(&list);

    // NOTE: Checks for the fail results might be great too

    // Insert first value
    jsonLinkedListInsert(&list, 0, &VALUE_ONE);
    // Check if the size properly reflects the amount of nodes in the list
    CuAssertIntEquals(test, 1, list->size);
    // Check if the node got properly assigned as the start of the list
    CuAssertPtrNotNull(test, list->start);
    
    // Value check
    void *firstValuePtr;
    jsonLinkedListAt(&list, 0, &firstValuePtr);
    int firstValue = *((int*)firstValuePtr);
    // Check if the retrieved value corresponds to the value that was supposed to be inserted
    CuAssertIntEquals(test, VALUE_ONE, firstValue);

    // Insert second value
    jsonLinkedListInsert(&list, 1, &VALUE_TWO);
    // Check if the size properly reflects the amount of nodes in the list
    CuAssertIntEquals(test, 2, list->size);
    // Check if the node got properly appended to the list
    CuAssertPtrNotNull(test, list->start->next);
    
    // Value check
    void *secondValuePtr;
    jsonLinkedListAt(&list, 1, &secondValuePtr);
    int secondValue = *((int*)secondValuePtr);
    // Check if the retrieved value corresponds to the value that was supposed to be inserted
    CuAssertIntEquals(test, VALUE_TWO, secondValue);

    // Replace second value with other value
    jsonLinkedListInsert(&list, 1, &VALUE_THREE);
    // Check that the size didn't change because no new node was supposed to be appended to the list
    CuAssertIntEquals(test, 2, list->size);
    CuAssertPtrNotNull(test, list->start->next);
    
    // Value check
    void *thirdValuePtr;
    jsonLinkedListAt(&list, 1, &thirdValuePtr);
    int thirdValue = *((int*)thirdValuePtr);
    // Check that the retrieved value corresponds to the value that was supposed to replace the previous value
    CuAssertIntEquals(test, VALUE_THREE, thirdValue);
    // Check that no node was added to the list
    CuAssertPtrEquals(test, NULL, list->start->next->next);
}
void TestListRemove(CuTest *test){}

void TestListAt(CuTest *test)
{
    JSONLinkedList *list = NULL;
    jsonLinkedListCreate(&list);

    // NOTE: Checks for the fail results might be great too

    // Insert several values to the list so that something can actually be retrieved from it
    jsonLinkedListInsert(&list, 0, &VALUE_ONE);
    jsonLinkedListInsert(&list, 1, &VALUE_TWO);
    jsonLinkedListInsert(&list, 2, &VALUE_THREE);

    // Checks that all nodes got appended correctly
    CuAssertIntEquals(test, 3, list->size);
    CuAssertPtrEquals(test, NULL, list->start->next->next->next);

    // Retrieve value
    void *valPtr;
    jsonLinkedListAt(&list, 2, &valPtr);
    int val = *((int*)valPtr);
    // Check that the value at the index is what was inserted
    CuAssertIntEquals(test, VALUE_THREE, val);
}
void TestListContains(CuTest *test){}

void TestListPushFront(CuTest *test){}
void TestListPopFront(CuTest *test){}

void TestListPushBack(CuTest *test)
{
    
}
void TestListPopBack(CuTest *test)
{

}

void TestListToArray(CuTest *test){}

void TestListFree(CuTest *test)
{
    JSONLinkedList *list = NULL;
    jsonLinkedListCreate(&list);

    jsonLinkedListFree(&list);
}