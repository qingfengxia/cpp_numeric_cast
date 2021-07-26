[![Travis Ubuntu Build Status](https://travis-ci.org/qingfengxia/cpp_numeric_cast.svg?branch=master)](https://travis-ci.org/qingfengxia/cpp_numeric_cast.svg)   [![Build Status](https://dev.azure.com/qingfengxia-github/qingfengxia/_apis/build/status/qingfengxia.cpp_numeric_cast%20(1)?branchName=master)](https://dev.azure.com/qingfengxia-github/qingfengxia/_build/latest?definitionId=2&branchName=master)

[![codecov](https://codecov.io/gh/qingfengxia/cpp_numeric_cast/branch/master/graph/badge.svg)](https://codecov.io/gh/qingfengxia/cpp_numeric_cast)

[![Boost Software License][license-badge]](LICENSE.txt)

# Development and demonstration of some cpp proposals

This repository holds code demonstration related safe numerics

### Download the header-only library and use it in your project

[Boost Software License](https://www.boost.org/users/license.html). It is free for open source and close source project.


## Proposals
### Runtime safe `numeric_cast`

usage:
```c++
long l = INT_MAX + (long)1;
int i = numeric_cast<>(1);  // this will throw overflow exception!
auto b = is_convertable<int>(l);  // gives false, let you test overflow without using C++ exception

is_numeric<T>::value  // can be used as in template, inside std::enable_if<>
```

[proposal: numeric_cast](proposal_numeric_cast.md)

### Reuse keyword `explicit` to prevent implicit conversion of function parameter

[proposal: Reuse keyword `explicit` to prevent implicit conversion of function parameter](proposal_explicit.md)



## Misc
### Demonstration of `has_member()` type trait

It has already introduced in C++20, here there is a demo to use them  [has_member.h](has_member.h), and also collected solution from Stackoverflow, to implement in C++03, C++11.
