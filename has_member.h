/** 
 * https://stackoverflow.com/questions/257288/templated-check-for-the-existence-of-a-class-member-function
 * C++ allows SFINAE to be used for this 
 *  1. C++ 20, requires
 *  2. C++17 std::is_detected<>  will not merged into C++ as C++20 requires
 *  3. C++14 boost::Hana, when generic lambda has been supported in C++14
 *  `auto has_toString = hana::is_valid([](auto&& obj) -> decltype(obj.toString()) { });`
 *  4. C++11: 
 *  5. pre C++11: use the followig macro
 * (notice that with C++11 features this is simplier ,
 * because it supports extended SFINAE on nearly arbitrary expressions
 *  - the below was crafted to work with common C++03 compilers):
 * 
 *  has_variable, is not so useful as field is preferred to be protected
 * */

#include <string>

#if __cplusplus > 201703L
// C++ requires clause
template<class T>
std::string optionalToString(T* obj)
{
    constexpr bool has_toString = requires(const T& t) {
        t.toString();
    };

    if constexpr (has_toString)
        return obj->toString();
    else
        return "toString not defined";
}
#endif


// macro function works for C++03
#define HAS_MEM_FUNC(func, name)                                        \
    template<typename T, typename Signature>                                 \
    struct name {                                                       \
        typedef char yes[1];                                            \
        typedef char no [2];                                            \
        template <typename U, U> struct type_check;                     \
        template <typename _1> static yes &chk(type_check<Signature, &_1::func > *); \
        template <typename   > static no  &chk(...);                    \
        static bool const value = sizeof(chk<T>(0)) == sizeof(yes);     \
    }

#if __cplusplus < 201103L
namespace std{
    template<bool C, typename T = void>
    struct enable_if {
      typedef T type;
    };

    template<typename T>
    struct enable_if<false, T> { };
}
#else
#include <type_traits>
#endif

