//demo_explicit.cpp

#include <vector>
#include <iostream>
#include <limits>
#include <type_traits>



/*
struct Money {};

template <typename T>
Money & operator*=(const T & n)
{
  static_assert(std::is_integral<T>::value, "Error: can only multiply money by integral amounts!");
  // ...
}
*/

#if __cplusplus > 201703L 
template <typename T> 
void g(T, std::type_identity_t<T>)
{
    std::cout << "g(T, std::type_identity_t<T>) called \n";
}
#endif

/// prevent implicit conversion for all parameters (needs C++11)
#define DISABLE_IMPLICIT_CASTS(func_name) \
    template <typename... T>              \
    void func_name(T...) = delete

void f_noconvert(int i)
{
    std::cout << "f(int i) called \n";
}
DISABLE_IMPLICIT_CASTS(f_noconvert);


void f_2(int i, std::string name)
{
    std::cout << "f_2(int i, std::string name) called \n";
}

void f_2_noconvert(int i, double j)
{
    std::cout << "f_2_nonconvert(int i, double j) called \n";
}
void f_2_noconvert(int i, std::string name)
{
    std::cout << "f_2_nonconvert(int i, std::string name) called \n";
}
DISABLE_IMPLICIT_CASTS(f_2_noconvert);

template <typename T>  
void print_vector(std::vector<T> v)
{
    //std::cout << "f(std::vector v) called \n";
    std::cout << "[ ";
    for(auto& i: v)
        std::cout << i << ", ";
    std::cout << "]\n";
}


void print_double_vector(std::vector<double> v)
{
    //std::cout << "f(std::vector v) called \n";
    std::cout << "[ ";
    for(auto& i: v)
        std::cout << i << ", ";
    std::cout << "]\n";
}

int main() 
{
    #if __cplusplus > 201703L 
    g(1, 2);
    g(1, 1.2); // std::identity_type_t  can let implicit conversion happen
    #endif

    //print_vector({3});
    //std::vector<int> v=1; // explicit contructor forbid this 
    std::vector<int> v1={1};
    print_vector(v1);
    std::vector<double> v2{2};
    print_vector(v2);
    std::vector<double> v3(3.5); // explicit noconvert is better here to avoid silly error
    print_vector(v3);
    print_vector(std::vector<double>(4.5));

    f_noconvert(4);


    f_2(1.0, "test");  // implicit convert from const char* to std::string
    //f_2_noconvert(1, "test");
    //f_2_noconvert(1.0, std::string("test"));
    f_2_noconvert(1, std::string("test"));
    f_2_noconvert(1, 3.5);  // overload

}