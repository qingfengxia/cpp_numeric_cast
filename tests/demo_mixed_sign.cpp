#include <vector>
#include <iostream>
#include <limits>
// -Wsign-conversion -Wall -Wextra

// typeid().name() of g++ does not given full type name
#include "../third-party/type_name.h"

void test_implicit_conversion()
{
    uint64_t u64 = 100UL;
    int64_t i64 = 100L;
    int32_t i32 = 1;
    uint32_t u32 = 1U;
    // from i32 to i64 is safe numeric promotion
    std::cout << type_name(u64) << " + " << type_name(i32) << 
        " = " << type_name(u64+i32) << std::endl;
    std::cout << type_name(u32) << " + " << type_name(i32) << 
        " = " << type_name(u32+i32) << std::endl;
    std::cout << type_name(u64) << " + " << type_name(i64) << 
        " = " << type_name(u64+i64) << std::endl;
}

int main() {
    test_implicit_conversion();

    uint64_t big = std::numeric_limits<size_t>::max() - 1UL;
    int32_t i_big = big;  // narrowing, implicit conversion, warning is desired
    std::cout << "i_big = " << i_big<< std::endl;

    // visual studio 2019 has a warning
    uint64_t u_minus = -1;  // warning is desired for this out of range assignment
    std::cout << "i_minus= " << u_minus<< std::endl;

    auto myVector = std::vector<int>{1, 2, 3};
    int i = myVector.size();  // - 100;
    //int j = static_cast<int>(myVector.size() - 100);
    std::cout << "myVector.size() - 100 = " << i  << std::endl;
    
    int signed_index = myVector.size() - 100;
    std::cout << "signed_index= " << signed_index << std::endl;
    // implicit conversion happens during indexing the vector
    std::cout << "myVector[signed_index]= " <<myVector[signed_index] << std::endl;

    unsigned unsigned_index = myVector.size() - 100;
    // warning: comparison of unsigned expression >= 0 is always true [-Wtype-limits]
    if (unsigned_index >=0)
    {
    std::cout << "unsigned_index= " << unsigned_index << std::endl;
    std::cout << "myVector[unsigned_index]= " <<myVector[unsigned_index] << std::endl;
    }
}