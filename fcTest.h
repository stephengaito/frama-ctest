

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

#ifdef __FRAMAC__
// We need to stub out the POSIX signal handlers sigsetjmp and siglongjmp
#include "__fc_builtin.h"
#define signal(signalInt, signalHandler)
#define sigsetjmp(sigJmpBuf, saveSigs)  Frama_C_interval(0, 2)
#define siglongjmp(sigJmpBuf, retVal)
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