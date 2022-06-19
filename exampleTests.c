#include <signal.h>
#include "fctest.h"

void suite1Test1(FCTestCase *tc, void*) {
	fcTestNotNull(tc, NULL, "should fail", FALSE);
	fcTestNull(tc, NULL, "should pass", FALSE);
}

void suite1Test2(FCTestCase *tc, void*) {
	fcTestNotNull(tc, NULL, "should fail", TRUE);
	fcTestNull(tc, NULL, "should pass", FALSE);
}

void suite1Test3(FCTestCase *tc, void*) {
	raise(SIGSEGV);
}

int main(int argc, const char *argv[]) {

  fcAddTest(TRUE, "Suite1", "Test1", suite1Test1, NULL, NULL, NULL);
  fcAddTest(TRUE, "Suite1", "Test2", suite1Test2, NULL, NULL, NULL);
  fcAddTest(TRUE, "Suite1", "Test3", suite1Test3, NULL, NULL, NULL);

	return fcTestRunner(argc, argv);
}

#ifdef __FRAMAC__

int evaMain(void) {
  char *argv[3];
  argv[0] = "evaMain";
  argv[1] = "Suite1";
  argv[2] = NULL;

  return main(2, argv);
}

#endif
