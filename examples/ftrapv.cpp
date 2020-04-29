/**
source:  https://gist.github.com/mastbaum/1004768
A demonstration of GCC's ftrapv flag for C++ integer overflow debugging
*

#include<iostream>
#include<signal.h>
#include<limits.h>

/** g++'s -ftrapv flag provides some protection against integer overflows. It
 *  is a little awkward to use, though. All it will do is "trap" -- you must
 *  provide a signal handler to deal with it.
 *
 *  (You must compile with -ftrapv for this to work)
 */

// a simple signal handler. it must take the signal as an argument, per
// signal.h, whether we use it or not.
void handler(int /*signal*/)
{
  std::cout << "Overflowed!" << std::endl;
}

int main()
{
  // when we get a SIGABRT, call handler
  signal(SIGABRT, &handler);

  // LONG_MAX is the largest long integer on this system, from limits.h
  long a = LONG_MAX;
  int b = 1;
  long c = a + b;

  return 0;
}