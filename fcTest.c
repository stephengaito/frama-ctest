
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>  // see: man 7 signal-safety

#include "fcTest.h"

///////////////////////////////////////////////////////////////////////
// manage tests

typedef struct fctest_struct {
  struct fctest_struct *next;
  int                   runTest;
  const char           *suiteName;
  const char           *testName;
  fctest_run_func      *testRunner;
  void                 *testData;
  fctest_setup_func    *setupTest;
  fctest_teardown_func *teardownTest;
} FCTest;

FCTest *fcTests = NULL;

/*@
  requires \valid_read(suiteName);
  requires \valid_read(testName);
 */
void fcAddTest(
  int                   runTest,
  const char           *suiteName,
  const char           *testName,
  fctest_run_func       testRunner,
  void                 *testData,
  fctest_setup_func     setupTest,
  fctest_teardown_func  teardownTest
) {
  FCTest *newTest = (FCTest*) calloc(1, sizeof(FCTest));
  if (! newTest) return ; // we can not add this test!

  newTest->next         = NULL;
  newTest->runTest      = runTest;
  newTest->suiteName    = suiteName;
  newTest->testName     = testName;
  newTest->testRunner   = testRunner;
  newTest->testData     = testData;
  newTest->setupTest    = setupTest;
  newTest->teardownTest = teardownTest;

  FCTest *lastTest = fcTests;

  if (!lastTest) {
    fcTests = newTest;
    return;
  }

  while(lastTest->next) lastTest = lastTest->next;
  lastTest->next = newTest;
}

///////////////////////////////////////////////////////////////////////
// assertions

static sigjmp_buf fcTestJmp;

void fcSignalHandler(int signum) {
  siglongjmp(fcTestJmp, 2);
}

void fcTestNull(
  FCTestCase* tc,
  void* actual,
  const char* msg,
  int abortTest
) {
  if (actual) {
    printf("    fcTestNull: expected %p to be NULL: %s\n", actual, msg);
    tc->failed++;
    if (abortTest) siglongjmp(fcTestJmp, 1);
  } else tc->passed++;
}

/*@
  requires \valid(tc);
  assigns *tc;
  behavior actual_not_null:
    assumes actual != \null;
    assigns tc->passed \from indirect:actual;
  behavior actual_null:
    assumes actual == \null;
    assigns tc->failed \from indirect:actual;
 */
void fcTestNotNull(
  FCTestCase* tc,
  void* actual,
  const char* msg,
  int abortTest
) {
  if (!actual) {
    printf("    fcTestNull: expected %p to not be NULL: %s\n", actual, msg);
    tc->failed++;
    if (abortTest) siglongjmp(fcTestJmp, 1);
  } else tc->passed++;
}

///////////////////////////////////////////////////////////////////////
// run test suites

/*@
  requires \valid_read(argv[0 .. (argc-1)]);
 */
int fcTestRunner(int argc, const char *argv[]) {
  int passedTests   = 0;
  int failedTests   = 0;
  int abortedTests  = 0;
  int skippedTests  = 0;
  int signaledTests = 0;

  signal(SIGSEGV, fcSignalHandler);

  FCTest *curTest = fcTests;
  while(curTest) {

    if (1 < argc) {
      int suiteFound = FALSE;
      for (int i = 1; i < argc; i++) {
        if (strcasecmp(curTest->suiteName, argv[i]) == 0) {
          suiteFound = TRUE;
          break;
        }
      }
      if (!suiteFound) {
        curTest = curTest->next;
        continue;
      }
    }

    printf("\n%s::%s:\n", curTest->suiteName, curTest->testName);
    if (!curTest->runTest) {
      skippedTests++;
      printf("  %s\n", "skipped");
      continue;
    }
    if (curTest->setupTest)    curTest->setupTest(curTest->testData);
    int testResult = sigsetjmp(fcTestJmp, 1);
    if (testResult == 0) {

      FCTestCase tc;
      tc.failed = 0;
      tc.passed = 0;
      if (curTest->testRunner) curTest->testRunner(&tc, curTest->testData);
      printf("  %d passed, %d failed\n", tc.passed, tc.failed);
      if (tc.failed) {
        failedTests++;
    	  printf("  %s\n", "FAILED");
      } else {
        passedTests++;
    	  printf("  %s\n", "PASSED");
      }
    } else if (testResult == 1) {
      abortedTests++;
    	printf("  %s\n", "ABORTED");
    } else {
      signaledTests++;
      printf("  %s\n", "SEGMENT FAULT");
    }
    if (curTest->teardownTest) curTest->teardownTest(curTest->testData);
    curTest = curTest->next;
  }

  printf("\n------------------\n");
  printf("Tests:\n");
  printf("   skipped: %d\n", skippedTests);
  printf("  --------------\n");
  printf("    passed: %d\n", passedTests);
  printf("    failed: %d\n", failedTests);
  printf("   aborted: %d\n", abortedTests);
  printf("  signaled: %d\n", signaledTests);
  printf("  --------------\n");
  printf("     total: %d\n",
    (passedTests+failedTests+abortedTests+skippedTests+signaledTests)
  );
  printf("------------------\n");
  return failedTests+abortedTests+signaledTests;
}
