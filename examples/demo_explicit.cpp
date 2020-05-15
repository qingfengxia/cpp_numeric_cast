//demo_explicit.cpp

#include <vector>
#include <iostream>
#include <limits>
#include <type_traits>


struct Value {
    double v;
    Value(double _v) : v(_v){};
};

struct MyValues {
    std::vector<double> values;
    MyValues(int size) {  };
    MyValues(Value) {  /* bad design, may lead to bugs */ };
    MyValues(std::vector<double> v) { values = v; };
};

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
    std::cout << "vector [ ";
    for(size_t i=0; i<v.size(); i++)
    {
        std::cout << v[i];
        if (i < v.size() -1)
            std::cout << ", "  ;
    }
    std::cout << " ]\n";
}
//DISABLE_IMPLICIT_CASTS(print_vector);


void print_double_vector(std::vector<double> v)
{
    //std::cout << "f(std::vector v) called \n";
    std::cout << "vector [ ";
    for(size_t i=0; i<v.size(); i++)
    {
        std::cout << v[i];
        if (i < v.size() -1)
            std::cout << ", "  ;
    }
    std::cout << " ]\n";
}

#if __cplusplus > 201903L 
template <typename T> 
void g(T, std::type_identity_t<T>)
{
    std::cout << "g(T, std::type_identity_t<T>) called \n";
}
#endif

int main() 
{
    MyValues mv(1.5);  // there is no constructor of double type
    //MyValues mv2{1.5};  // no compiler error if MyValues(int size) is not defined, but dangerous
    MyValues mv2{5};  // no compiler error, but dangerous
    //MyValues mv2 = 2.5;  // 2 conversion needed, so forbidden by compiler
    //MyValues mv({1.5});

    //print_vector({3});  template argument deduction/substitution failed:

    //std::vector<int> v=1; // explicit contructor forbid this implicit conversion 
    std::vector<double> v(1.5); 
    print_vector(v);
    std::vector<double> v_1({1.5});  
    print_vector(v_1);

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


    #if __cplusplus > 201903L 
    g(1, 2);
    g(1, 1.2); // std::identity_type_t  can let implicit conversion happen
    #endif

}