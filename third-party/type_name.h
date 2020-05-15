#pragma once

#include <string>

#ifdef __GNUG__

    #include <cxxabi.h>
    #include <cstdlib>
    #include <memory>

    template< typename T > std::string type_name()
    {
        int status ;
        std::unique_ptr< char[], decltype(&std::free) > buffer(
            __cxxabiv1::__cxa_demangle( typeid(T).name(), nullptr, 0, &status ), &std::free ) ;
        return status==0 ? buffer.get() : "__cxa_demangle error" ;
    }

#else // !defined __GNUG__

    template< typename T > std::string type_name() { return typeid(T).name() ; }

#endif //__GNUG__

    template< typename T > std::string type_name( const T& ) { return type_name<T>() ; }