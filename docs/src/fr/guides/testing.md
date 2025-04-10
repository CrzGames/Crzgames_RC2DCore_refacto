
# Unit Testing

Unit testing is a critical part of the development process in RC2D, ensuring that individual components function as expected. RC2D leverages the `doctest` framework to facilitate these tests. Here is how you can work with unit tests in your RC2D projects.

## Overview

1. **Execution:** Unit tests are executed each time the executable is run, given that `-DENABLE_DOCTEST=ON` is specified.
2. **Supported Platforms:** Unit tests can be executed on Windows, macOS, and Linux platforms.
3. **Test Inclusion:** Tests must be included within an include file or source file.
4. **Excluding Tests:** Exclude unit tests from the executable by specifying `-DENABLE_DOCTEST=OFF` flag to CMake during build configuration.

## Writing a Unit Test

Unit tests are written in source files using the `doctest` framework. Below is a basic example of how a test case might look:

```cpp
#include <doctest/doctest.h>

TEST_CASE("Example test case") {
    int x = 1;
    CHECK(x == 1); // This test will pass.
    CHECK(x == 2); // This test will fail.
}
```

::: tip NOTE
It is essential to structure your tests to cover as much of the codebase as possible to ensure comprehensive testing coverage.
:::

::: warning IMPORTANT
Remember, unit tests are only executed on Windows, macOS, and Linux. Make sure to enable them during development to catch and resolve issues early.
:::

By integrating unit tests into your development workflow, you can significantly improve the reliability and quality of your game, ensuring that each component performs as expected.
