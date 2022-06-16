

#ifndef FCTEST_H
#define FCTEST_H

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct fcTestCase_struct {
	int passed;
	int failed;
} FCTestCase;

typedef void (fctest_run_func)(FCTestCase*, void*);
typedef void (fctest_setup_func)(void*);
typedef void (fctest_teardown_func)(void*);

void fcAddTest(
  int                   runTest,     // only run this test if TRUE
  const char           *suiteName,   // the name of the suite of tests
  const char           *testName,    // the name of this test
  fctest_run_func       testRunner,  // the test function
  void                 *testData,    // some test data
  fctest_setup_func     setupTest,   // a setup test function
  fctest_teardown_func  teardownTest // a teardown test function
);

void fcTestNull(FCTestCase* tc, void* expected, const char* msg, int abortTest);
void fcTestNotNull(FCTestCase* tc, void* expected, const char* msg, int abortTest);

int fcTestRunner(int argc, const char *argv[]);

#endif