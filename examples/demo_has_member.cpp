
#include <iostream>
#include "../has_member.h"


// it is good to declare this in cpp file
HAS_MEM_FUNC(toString, has_to_string);

template<typename T> 
typename std::enable_if<has_to_string<T, 
                   std::string(T::*)()>::value, std::string>::type
doSomething(T * t) {
   /* something when T has toString ... */
   return t->toString();
}

template<typename T> 
typename std::enable_if<!has_to_string<T, 
                   std::string(T::*)()>::value, std::string>::type
doSomething(T * t) {
   /* something when T doesnt have toString ... */
   return "T::toString() does not exist.";
}

struct A{ 
    std::string toString(){  return "toString() called"; }
};

int main()
{
// microsoft jump from 199711L directly to C++14, no C++11
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    std::cout << "C++11 is supported\n";
#else
    std::cout << "C++11 is not supported\n";
#endif
    
    A a;
    std::cout << doSomething(&a);
    
}
