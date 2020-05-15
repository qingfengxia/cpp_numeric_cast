//safe_at
#pragma once

#include "numeric_cast.h"
#include <vector>
#include <list>
#include <valarray>
#include <map>

#if __cplusplus > 201703L  && __has_include(<span>)  // C++20
#include <span>
#include <stdexcept>
#endif

namespace std {

#if __cplusplus > 201703L  && __has_include(<span>)  // C++20

    template <size_t _index, typename _T>
    const _T&  get(const span<_T>& seq)
    {
        if ( index < seq.size())
            return seq[_index];
        else
            throw std::out_of_range("index is beyond the size of the span<>");
    }

    //todo: at() for `std::span`, this must be implemented in that class
#endif


/*  not working yet, can someone help
    /// general getter for T[element_count] array types
    template <typename T, template<typename T> SequenceType, typename SourceType>
    const T&  get(const SourceType index)
    {
        //std::out_of_range is thrown.
    }
*/

    /// std::get(std::array<>) has 3 overloading, why?
 
    template <size_t _index, typename _T>
    const _T&  get(const vector<_T>& seq)
    {
        return seq.at(_index);  // std::out_of_range is thrown if index out of bound
    }

    template <size_t _index, typename _T>
    const _T&&  get(const vector<_T>&& seq)
    {
        return move(seq.at(_index));  // std::out_of_range is thrown if index out of bound
    }

    /// todo: there are other overloading version
    template <size_t _index, typename _T>
    const _T&  get(const valarray<_T>& seq)
    {
        if ( _index < seq.size())
            return seq[_index];
        else
            throw std::out_of_range("index is beyond the size of the valarray<>");
    }

    /// std::get<key_value> for std::map, not so useful
    template <typename _KeyType,  _KeyType key_value, typename _T> inline
    const _T&  __get(const map<_KeyType, _T> _hash)
    {
        return _hash.at(key_value);
    }

    template <typename _KeyType,  _KeyType key_value, typename _T> inline
    const _T&  get(const map<_KeyType, _T> _hash)
    {
        return _hash.at(key_value);
    }

};