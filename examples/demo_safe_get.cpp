
#include "../safe_get.h"

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <valarray>
#include <map>
#include <unordered_map>
#include <array>
// stack and queue has no at() or [] indexing
//#include <queue>
//#include <stack>

#if __cplusplus > 201703L  && __has_include(<span>)  // C++20
// clang 9 or gcc 10
#include <span>
#endif

//template<typename T=int>
void test_sequence()
{
    typedef int DT;
    //using namespace std;
    auto myTuple = std::make_tuple(1, 2, 3);
    DT arr[3] = {1, 2, 3};
    std::vector<DT> myVector{1, 2, 3};
    std::list<DT> myList{1, 2, 3};
    std::array<DT, 3> myArray{1, 2, 3};
    std::valarray<DT> myValarray{1, 2, 3};


    const size_t i = 1;
    std::cout << "std::get<i>(myVector) = " << std::get<i>(myVector) << std::endl;
    //DT&& value = std::get<i>(myVector);  // overloading

    //std::cout << "std::get<i>(rawArray)" << std::get<i>(arr) << std::endl;
    
    std::cout << "std::get<i>(myArray) =" << std::get<i>(myArray) << std::endl;
    std::cout << "std::get<i>(myTuple) =" << std::get<i>(myTuple) << std::endl;
    //cout << "std::get<i>(myList) =" << std::get<i>(myList) << endl;
    std::cout << "std::get<i>(myValarray) =" << std::get<i>(myValarray) << std::endl;
    // size_t template type accept int constant, but not int variable
    std::cout << "std::get<1>(myValarray) =" << std::get<1>(myValarray) << std::endl;
    int i_int = 2;
    //cout << "std::get<1>(myValarray) =" << std::get<i_int>(myValarray) << endl;

#if __cplusplus > 201703L  && __has_include(<span>)  // C++20
    span mySpan{arr, std::size(arr)};
    cout << "std::get<i>(mySpan) =" << std::get<i>(mySpan) << endl;
#endif

}

void test_map()
{
    typedef int DT;
    //using namespace std;

    //std::map<string, DT> myMap{{"k1", 1}, {"k2", 2}, {"k3", 3}};
    const std::string myKey = "k1";
    typedef size_t KT;  // KD can not be string, which is template
    const size_t ki = 1;
    std::map<KT, DT> myMap{{1, 1}, {1, 2}, {1, 3}};
    std::cout << std::get<KT, ki>(myMap) << std::endl;
}



int main()
{

    std::cout << std::boolalpha;
    
    test_sequence();
}