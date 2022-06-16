# Frama-C testing framework

A very simple [Frama-C](https://frama-c.com/) friendly ANSI-C testing
framework.

You should not need a testing framework with Frama-C proven code, however
it helps the overall proof process to be able to call small isolated parts
of the code you want to prove correct using Frama-C. This is what a
testing framework allows you to do. Unfortunately most C test frameworks
make extensive use of ANSI-C macros. Frama-C highlights all macro
expansions in red which is distracting.

This testing framework uses all pure ANSI-C with no macros.

To use this framework, copy the `fctest.c` and `fctest.h` files into your
test code directory. We use the MIT license to make this easy to do.

You must also declare a `main` function in *one* of your test files. This
`main` function should have the following form:

```
int main(int argc, const char argv[]) {

  fcAddTest(TRUE, "Suite1", "Test1", suite1Test1, NULL, NULL, NULL);
  fcAddTest(TRUE, "Suite1", "Test2", suite1Test2, NULL, NULL, NULL);
  fcAddTest(TRUE, "Suite1", "Test3", suite1Test3, NULL, NULL, NULL);

	return fcTestRunner(argc, argv);
}
```

where `fcAddTest` has the following prototype:

```
void fcAddTest(
  int                   runTest,     // only run this test if TRUE
  const char           *suiteName,   // the name of the suite
  const char           *testName,    // the name of the test
  fctest_run_func       testRunner,  // the test function
  void                 *testData,    // some test data
  fctest_setup_func     setupTest,   // a setup test function
  fctest_teardown_func  teardownTest // a teardown test function
);
```
Any or all of `testData`, `setupTest` and `teardownTest` can be `NULL`.

A `testRunner` must have the following prototype:

```
void ATest(FCTestCase* tc, void* data, const char* aMessage, int abortTest);
```

Where `tc` is used to track the number of assertions which pass or fail.
The `data` argument is a `(void*)`, which might be `NULL`, which points to
any test data. The `aMessage` argument is a message to be printed if this
(sub)test fails. If the `abortTest` arugment is `TRUE` then the whole test
case is aborted. If the `abortTest` arugment is `FALSE` then subsequent
(sub)tests in the current test case will be tested.

Both the `setupTest` and `teardownTest` functions have the following
prototype:

```
void SetupTeardownTest(void* data);
```

Finally any command line arguments are assumed to be the names of test
suites to be run. If there are not command line arguments, then all test
suites will be run.
